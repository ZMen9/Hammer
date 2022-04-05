#include "hmpch.h"
#include "Hammer/Scene/Scene.h"
#include <glm/glm.hpp>
#include "Hammer/Scene/Entity.h"
#include "Hammer/Scene/Components.h"
#include "Hammer/Renderer/Renderer2D.h"
#include "Hammer/Scene/ScriptableEntity.h"
// Box2D phyics
#include <box2d/b2_body.h>
#include <box2d/b2_world.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>

// About entt::view and entt::group:
// Views are a non-intrusive tool to access entities and components without
// affecting other functionalities or increasing the memory consumption.  Groups
// are an intrusive tool that allows to reach higher performance along critical
// paths but has also a price to pay for that.
namespace hammer {

static b2BodyType Rigidbody2DTypeToBox2DBody(
    Rigidbody2DComponent::BodyType body_type) {
  switch (body_type) {
    case hammer::Rigidbody2DComponent::BodyType::STATIC:
      return b2_staticBody;
      break;
    case hammer::Rigidbody2DComponent::BodyType::DYNAMIC:
      return b2_dynamicBody;
      break;
    case hammer::Rigidbody2DComponent::BodyType::KINEMATIC:
      return b2_kinematicBody;
      break;
  }
  HM_CORE_ASSERT(false, "Unknown body type");
  return b2_staticBody;
}

Scene::Scene() {}

Scene::~Scene() { 
}

Entity Scene::CreateEntity(const std::string& name /*= std::string()*/) {
  return CreateEntityWithUUID(UUID(), name);
}

Entity Scene::CreateEntityWithUUID(
    UUID uuid, const std::string& name /*= std::string()*/) {
  Entity entity{registry_.create(), this};
  entity.AddComponent<IDComponent>(uuid);
  entity.AddComponent<TransformComponent>();
  auto& tag = entity.AddComponent<TagComponent>();
  tag.tag = name.empty() ? "Entity" : name;
  return entity;
}

void Scene::DestroyEntity(Entity entity) { registry_.destroy(entity);}

void Scene::OnRuntimeStart() {
  // b2World(gravity)
  physics_world_ = new b2World({0.0f, -9.8f});
  auto view = registry_.view<Rigidbody2DComponent>();
  for (auto e : view) {
    Entity entity{e, this};
    auto& transform = entity.GetComponent<TransformComponent>();
    // Set Rigidbody
    auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
    b2BodyDef body_def;
    body_def.type = Rigidbody2DTypeToBox2DBody(rb2d.type);
    body_def.position.Set(transform.translation.x, transform.translation.y);
    body_def.angle = transform.rotation.z;

    b2Body* body = physics_world_->CreateBody(&body_def);
    body->SetFixedRotation(rb2d.fixed_rotation);
    rb2d.runtime_body = body;

    if (entity.HasComponent<BoxCollider2DComponent>()) {
      auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

      b2PolygonShape box_shape;
      box_shape.SetAsBox(bc2d.size.x * transform.scale.x,
                         bc2d.size.y * transform.scale.y);
      // Set BoxCollider
      b2FixtureDef fixture_def;
      fixture_def.shape = &box_shape;
      fixture_def.density = bc2d.density;
      fixture_def.friction = bc2d.friction;
      fixture_def.restitution = bc2d.restitution;
      fixture_def.restitutionThreshold = bc2d.restitution_threshold;
      body->CreateFixture(&fixture_def);
    }

    if (entity.HasComponent<CircleCollider2DComponent>()) {
      auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

      b2CircleShape circleShape;
      circleShape.m_p.Set(cc2d.offset.x, cc2d.offset.y);
      circleShape.m_radius = transform.scale.x * cc2d.radius;

      b2FixtureDef fixtureDef;
      fixtureDef.shape = &circleShape;
      fixtureDef.density = cc2d.density;
      fixtureDef.friction = cc2d.friction;
      fixtureDef.restitution = cc2d.restitution;
      fixtureDef.restitutionThreshold = cc2d.restitution_threshold;
      body->CreateFixture(&fixtureDef);
    }
  }
}

void Scene::OnRuntimeStop() {
  delete physics_world_;
  physics_world_ = nullptr;
}

void Scene::OnUpdateRuntime(Timestep ts) {
  //Update scripts
  {
    registry_.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
      // TODO: Move to Scene::OnScenePlay
      if (!nsc.instance) {
        nsc.instance = nsc.InstantiateScript();
        nsc.instance->entity_ = Entity{entity, this};
        nsc.instance->OnCreate();
      }
      nsc.instance->OnUpdate(ts);
    });
  }

  // Physics
  {
    constexpr int32_t velocityIterations = 6;
    constexpr int32_t positionIterations = 2;
    physics_world_->Step(ts, velocityIterations, positionIterations);

    // Retrieve transform from Box2D,change the transform by retrieve from rb.
    auto view = registry_.view<Rigidbody2DComponent>();
    for (auto e : view) {
      Entity entity = {e, this};
      auto& transform = entity.GetComponent<TransformComponent>();
      auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

      b2Body* body = (b2Body*)rb2d.runtime_body;
      const auto& position = body->GetPosition();
      transform.translation.x = position.x;
      transform.translation.y = position.y;
      transform.rotation.z = body->GetAngle();
    }
  }

  // Render 2D
  Camera* main_camera = nullptr;
  glm::mat4 camera_transform;
  { 
    auto view = registry_.view<TransformComponent, CameraComponent>(); 
    for (auto entity : view) {
      auto [transform, camera] =
          view.get<TransformComponent, CameraComponent>(entity);
      if (camera.primary) {
        main_camera = &camera.camera;
        camera_transform = transform.GetTransform();
        break;
      }
    }
  }

  if (main_camera) {
    Renderer2D::BeginScene(*main_camera, camera_transform);
    // Draw sprites
    {
      auto group = registry_.group<TransformComponent>(
          entt::get<SpriteRendererComponent>);
      for (auto entity : group) {
        auto [transform, sprite] =
            group.get<TransformComponent, SpriteRendererComponent>(entity);
        Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
      }
    }
    // Draw circles
    {
      auto view =
          registry_.view<TransformComponent, CircleRendererComponent>();
      for (auto entity : view) {
        auto [transform, circle] =
            view.get<TransformComponent, CircleRendererComponent>(entity);

        Renderer2D::DrawCircle(transform.GetTransform(), circle.color,
                               circle.thickness, circle.fade, (int)entity);
      }
    }
    

    Renderer2D::EndScene();
  }
}

void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera) {
  // Draw Sprites
  {
    Renderer2D::BeginScene(camera);
    auto group =
        registry_.group<TransformComponent>(entt::get<SpriteRendererComponent>);
    for (auto entity : group) {
      auto [transform, sprite] =
          group.get<TransformComponent, SpriteRendererComponent>(entity);
      Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
    }
  }
  // draw circle
  {
    auto view = registry_.view<TransformComponent, CircleRendererComponent>();
    for (auto entity : view) {
      auto [transform, circle] =
          view.get<TransformComponent, CircleRendererComponent>(entity);

      Renderer2D::DrawCircle(transform.GetTransform(), circle.color,
                             circle.thickness, circle.fade, (int)entity);
    }
  }
  /*Renderer2D::DrawLine(glm::vec3(2.0f), glm::vec3(5.0f),
                       glm::vec4(0.2f, 0.1f, 0.5f, 1.0f));
  Renderer2D::DrawRect(glm::vec3(0.0f), glm::vec2(5.0f, 7.0f),
                       glm::vec4(0.2f, 0.1f, 0.5f, 1.0f));*/

  Renderer2D::EndScene();
}

void Scene::OnViewportResize(uint32_t width, uint32_t height) {
  viewport_width_ = width;
  viewport_height_ = height;

  // Resize our non-FixedAspectRatio cameras
  auto view = registry_.view<CameraComponent>();
  for (auto entity : view) {
    auto& camera_component = view.get<CameraComponent>(entity);
    if (!camera_component.fixed_aspect_ratio)
      camera_component.camera.SetViewportSize(width, height);
  }
} 

Entity Scene::GetPrimaryCameraEntity() {
  auto view = registry_.view<CameraComponent>();
  for (auto entity : view) {
    const auto& camera = view.get<CameraComponent>(entity);
    if (camera.primary) return Entity(entity, this);
  }
  return {};
}

template<typename Component>
static void CopyComponent(entt::registry& dst, entt::registry& src,
  const std::unordered_map<UUID, entt::entity>& enttMap) {
  auto view = src.view<Component>();
  for (auto e : view) {
    UUID uuid = src.get<IDComponent>(e).id;
    HM_CORE_ASSERT(enttMap.find(uuid) != enttMap.end());
    entt::entity dstEnttID = enttMap.at(uuid);

    auto& component = src.get<Component>(e);
    dst.emplace_or_replace<Component>(dstEnttID, component);
  }
}

template <typename Component>
static void CopyComponentIfExists(Entity dst, Entity src) {
  if (src.HasComponent<Component>())
    dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
}

Ref<Scene> Scene::Copy(Ref<Scene> other) {
  Ref<Scene> newScene = CreateRef<Scene>();

  newScene->viewport_width_ = other->viewport_width_;
  newScene->viewport_height_ = other->viewport_height_;

  auto& srcSceneRegistry = other->registry_;
  auto& dstSceneRegistry = newScene->registry_;
  std::unordered_map<UUID, entt::entity> enttMap;

  // Create entities in new scene
  auto idView = srcSceneRegistry.view<IDComponent>();
  for (auto e : idView) {
    UUID uuid = srcSceneRegistry.get<IDComponent>(e).id;
    const auto& name = srcSceneRegistry.get<TagComponent>(e).tag;
    Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
    enttMap[uuid] = (entt::entity)newEntity;
  }

  // Copy components (except IDComponent and TagComponent)
  CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry,
                                    enttMap);
  CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry,
                                         enttMap);
  CopyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry,
                                         enttMap);
  CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
  CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry,
                                       enttMap);
  CopyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry,
                                      enttMap);
  CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry,
                                        enttMap);
  CopyComponent<CircleCollider2DComponent>(dstSceneRegistry, srcSceneRegistry,
                                        enttMap);

  return newScene;
}

void Scene::DuplicateEntity(Entity entity) {
  std::string name = entity.GetName();
  Entity new_entity = CreateEntity(name);

  CopyComponentIfExists<TransformComponent>(new_entity, entity);
  CopyComponentIfExists<SpriteRendererComponent>(new_entity, entity);
  CopyComponentIfExists<CircleRendererComponent>(new_entity, entity);
  CopyComponentIfExists<CameraComponent>(new_entity, entity);
  CopyComponentIfExists<NativeScriptComponent>(new_entity, entity);
  CopyComponentIfExists<Rigidbody2DComponent>(new_entity, entity);
  CopyComponentIfExists<BoxCollider2DComponent>(new_entity, entity);
  CopyComponentIfExists<CircleCollider2DComponent>(new_entity, entity);
}

template <typename T>
void Scene::OnComponentAdded(Entity entity, T& component) {
  static_assert(false);
}

template <>
void Scene::OnComponentAdded<IDComponent>(
    Entity entity, IDComponent& component) {}

template<>
void Scene::OnComponentAdded<TransformComponent>(
    Entity entity, TransformComponent& component) {}

template <>
void Scene::OnComponentAdded<TagComponent>(Entity entity,
                                           TagComponent& component) {}

template <>
void Scene::OnComponentAdded<CameraComponent>(Entity entity,
                                              CameraComponent& component) {
  if (viewport_width_ > 0 && viewport_height_ > 0)
    component.camera.SetViewportSize(viewport_width_, viewport_height_);

  /*entity.AddComponent<SpriteRendererComponent>();
  std::filesystem::path texturePath = "resources/icons/CameraFilm.png";
  Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
  if (texture->IsLoaded()) {
    entity.GetComponent<SpriteRendererComponent>().texture = texture;
  } else {
    HM_WARN("Could not load texture {0}", texturePath.filename().string());
  }*/
}

template <>
void Scene::OnComponentAdded<SpriteRendererComponent>(
    Entity entity, SpriteRendererComponent& component) {}

template <>
void Scene::OnComponentAdded<CircleRendererComponent>(
    Entity entity, CircleRendererComponent& component) {}

template <>
void Scene::OnComponentAdded<NativeScriptComponent>(
    Entity entity, NativeScriptComponent& component) {}


template <>
void Scene::OnComponentAdded<Rigidbody2DComponent>(
    Entity entity, Rigidbody2DComponent& component) {}

template <>
void Scene::OnComponentAdded<BoxCollider2DComponent>(
    Entity entity, BoxCollider2DComponent& component) {}

template <>
void Scene::OnComponentAdded<CircleCollider2DComponent>(
    Entity entity, CircleCollider2DComponent& component) {}
}  // namespace hammer