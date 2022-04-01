#include "hmpch.h"
#include "Hammer/Scene/Scene.h"
#include <glm/glm.hpp>
#include "Hammer/Scene/Entity.h"
#include "Hammer/Scene/Components.h"
#include "Hammer/Renderer/Renderer2D.h"

// About entt::view and entt::group:
// Views are a non-intrusive tool to access entities and components without
// affecting other functionalities or increasing the memory consumption.  Groups
// are an intrusive tool that allows to reach higher performance along critical
// paths but has also a price to pay for that.
namespace hammer {

Scene::Scene() {}

Scene::~Scene() {}

Entity Scene::CreateEntity(
    const std::string& name /*= std::string()*/) {
  Entity entity{registry_.create(), this};
  entity.AddComponent<TransformComponent>();
  auto& tag = entity.AddComponent<TagComponent>();
  tag.tag = name.empty() ? "Entity" : name;
  return entity;
}

void Scene::DestroyEntity(Entity entity) { registry_.destroy(entity);}

void Scene::OnUpdate(Timestep ts) {
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
    auto group =
        registry_.group<TransformComponent>(entt::get<SpriteRendererComponent>);

    for (auto entity : group) {
      auto [transform, sprite] =
          group.get<TransformComponent, SpriteRendererComponent>(entity);
      Renderer2D::DrawQuad(transform.GetTransform(), sprite.color);
    }
    Renderer2D::EndScene();
  }
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

template <typename T>
void Scene::OnComponentAdded(Entity entity, T& component) {
  static_assert(false);
}

template<>
void Scene::OnComponentAdded<TransformComponent>(
    Entity entity, TransformComponent& component) {}

template <>
void Scene::OnComponentAdded<TagComponent>(Entity entity,
                                           TagComponent& component) {}

template <>
void Scene::OnComponentAdded<CameraComponent>(Entity entity,
                                              CameraComponent& component) {
  component.camera.SetViewportSize(viewport_width_, viewport_height_);
}

template <>
void Scene::OnComponentAdded<SpriteRendererComponent>(
    Entity entity, SpriteRendererComponent& component) {}

template <>
void Scene::OnComponentAdded<NativeScriptComponent>(
    Entity entity, NativeScriptComponent& component) {}

}  // namespace hammer