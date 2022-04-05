#include "hmpch.h"
#include "Hammer/Scene/SceneSerializer.h"
#include <fstream>
#include <yaml-cpp/yaml.h>
#include "Hammer/Scene/Entity.h"
#include "Hammer/Scene/Components.h"
namespace YAML {
template <>
struct convert<glm::vec2> {
  static Node encode(const glm::vec2& rhs) {
    Node node;
    node.push_back(rhs.x);
    node.push_back(rhs.y);
    node.SetStyle(EmitterStyle::Flow);
    return node;
  }

  static bool decode(const Node& node, glm::vec2& rhs) {
    if (!node.IsSequence() || node.size() != 2) return false;

    rhs.x = node[0].as<float>();
    rhs.y = node[1].as<float>();
    return true;
  }
};
template<>
struct convert<glm::vec3> {
  static Node encode(const glm::vec3& rhs) {
    Node node;
    node.push_back(rhs.x);
    node.push_back(rhs.y);
    node.push_back(rhs.z);
    node.SetStyle(EmitterStyle::Flow);
    return node;
  }

  static bool decode(const Node& node, glm::vec3& rhs) {
    if (!node.IsSequence() || node.size() != 3) return false;

    rhs.x = node[0].as<float>();
    rhs.y = node[1].as<float>();
    rhs.z = node[2].as<float>();
    return true;
  }
};
template <>
struct convert<glm::vec4> {
  static Node encode(const glm::vec4& rhs) {
    Node node;
    node.push_back(rhs.x);
    node.push_back(rhs.y);
    node.push_back(rhs.z);
    node.push_back(rhs.w);
    node.SetStyle(EmitterStyle::Flow);
    return node;
  }

  static bool decode(const Node& node, glm::vec4& rhs) {
    if (!node.IsSequence() || node.size() != 4) return false;

    rhs.x = node[0].as<float>();
    rhs.y = node[1].as<float>();
    rhs.z = node[2].as<float>();
    rhs.w = node[3].as<float>();
    return true;
  }
};
}

namespace hammer {
YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v) {
  out << YAML::Flow;
  out << YAML::BeginSeq << v.x << v.y  << YAML::EndSeq;
  return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
  out << YAML::Flow;
  out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
  return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
  out << YAML::Flow;
  out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
  return out;
}

static std::string RigidBody2DBodyTypeToString(
    Rigidbody2DComponent::BodyType bodyType) {
  switch (bodyType) {
    case Rigidbody2DComponent::BodyType::STATIC:
      return "Static";
    case Rigidbody2DComponent::BodyType::DYNAMIC:
      return "Dynamic";
    case Rigidbody2DComponent::BodyType::KINEMATIC:
      return "Kinematic";
  }

  HM_CORE_ASSERT(false, "Unknown body type");
  return {};
}

static Rigidbody2DComponent::BodyType RigidBody2DBodyTypeFromString(
    const std::string& bodyTypeString) {
  if (bodyTypeString == "Static") return Rigidbody2DComponent::BodyType::STATIC;
  if (bodyTypeString == "Dynamic")
    return Rigidbody2DComponent::BodyType::DYNAMIC;
  if (bodyTypeString == "Kinematic")
    return Rigidbody2DComponent::BodyType::KINEMATIC;

  HM_CORE_ASSERT(false, "Unknown body type");
  return Rigidbody2DComponent::BodyType::STATIC;
}


SceneSerializer::SceneSerializer(const Ref<Scene> scene) 
		:scene_(scene) {}

// universally unique identifiers:uuids
static void SerializeEntity(YAML::Emitter& out, Entity entity) {
  HM_CORE_ASSERT(entity.HasComponent<IDComponent>());
  out << YAML::BeginMap;  // Entity
  out << YAML::Key << "Entity" << YAML::Value
      << entity.GetUUID();  

  if (entity.HasComponent<TagComponent>()) {
    out << YAML::Key << "TagComponent";
    out << YAML::BeginMap;  // TagComponent

    auto& tag = entity.GetComponent<TagComponent>().tag;
    out << YAML::Key << "Tag" << YAML::Value << tag;

    out << YAML::EndMap;  // TagComponent
  }

  if (entity.HasComponent<TransformComponent>()) {
    out << YAML::Key << "TransformComponent";
    out << YAML::BeginMap;  // TransformComponent

    auto& tc = entity.GetComponent<TransformComponent>();
    out << YAML::Key << "Translation" << YAML::Value << tc.translation;
    out << YAML::Key << "Rotation" << YAML::Value << tc.rotation;
    out << YAML::Key << "Scale" << YAML::Value << tc.scale;

    out << YAML::EndMap;  // TransformComponent
  }

  if (entity.HasComponent<CameraComponent>()) {
    out << YAML::Key << "CameraComponent";
    out << YAML::BeginMap;  // CameraComponent

    auto& cameraComponent = entity.GetComponent<CameraComponent>();
    auto& camera = cameraComponent.camera;

    out << YAML::Key << "Camera" << YAML::Value;
    out << YAML::BeginMap;  // Camera
    out << YAML::Key << "ProjectionType" << YAML::Value
        << (int)camera.GetProjectionType();
    out << YAML::Key << "PerspectiveFOV" << YAML::Value
        << camera.GetPerspectiveVerticalFOV();
    out << YAML::Key << "PerspectiveNear" << YAML::Value
        << camera.GetPerspectiveNearClip();
    out << YAML::Key << "PerspectiveFar" << YAML::Value
        << camera.GetPerspectiveFarClip();
    out << YAML::Key << "OrthographicSize" << YAML::Value
        << camera.GetOrthographicSize();
    out << YAML::Key << "OrthographicNear" << YAML::Value
        << camera.GetOrthographicNearClip();
    out << YAML::Key << "OrthographicFar" << YAML::Value
        << camera.GetOrthographicFarClip();
    out << YAML::EndMap;  // Camera

    out << YAML::Key << "Primary" << YAML::Value << cameraComponent.primary;
    out << YAML::Key << "FixedAspectRatio" << YAML::Value
        << cameraComponent.fixed_aspect_ratio;

    out << YAML::EndMap;  // CameraComponent
  }

  if (entity.HasComponent<SpriteRendererComponent>()) {
    out << YAML::Key << "SpriteRendererComponent";
    out << YAML::BeginMap;  // SpriteRendererComponent

    auto& spriteRendererComponent =
        entity.GetComponent<SpriteRendererComponent>();
    out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.color;

    out << YAML::EndMap;  // SpriteRendererComponent
  }

  if (entity.HasComponent<CircleRendererComponent>()) {
    out << YAML::Key << "CircleRendererComponent";
    out << YAML::BeginMap;  // SpriteRendererComponent

    auto& circleRendererComponent =
        entity.GetComponent<CircleRendererComponent>();
    out << YAML::Key << "Color" << YAML::Value << circleRendererComponent.color;
    out << YAML::Key << "Thickness" << YAML::Value << circleRendererComponent.thickness;
    out << YAML::Key << "Fade" << YAML::Value << circleRendererComponent.fade;

    out << YAML::EndMap;  // SpriteRendererComponent
  }

  if (entity.HasComponent<Rigidbody2DComponent>()) {
    out << YAML::Key << "Rigidbody2DComponent";
    out << YAML::BeginMap;  // Rigidbody2DComponent

    auto& rb2dComponent = entity.GetComponent<Rigidbody2DComponent>();
    out << YAML::Key << "BodyType" << YAML::Value
        << RigidBody2DBodyTypeToString(rb2dComponent.type);
    out << YAML::Key << "FixedRotation" << YAML::Value
        << rb2dComponent.fixed_rotation;

    out << YAML::EndMap;  // Rigidbody2DComponent
  }
  if (entity.HasComponent<BoxCollider2DComponent>()) {
    out << YAML::Key << "BoxCollider2DComponent";
    out << YAML::BeginMap;  // BoxCollider2DComponent

    auto& bc2dComponent = entity.GetComponent<BoxCollider2DComponent>();
    out << YAML::Key << "Offset" << YAML::Value << bc2dComponent.offset;
    out << YAML::Key << "Size" << YAML::Value << bc2dComponent.size;
    out << YAML::Key << "Density" << YAML::Value << bc2dComponent.density;
    out << YAML::Key << "Friction" << YAML::Value << bc2dComponent.friction;
    out << YAML::Key << "Restitution" << YAML::Value
        << bc2dComponent.restitution;
    out << YAML::Key << "RestitutionThreshold" << YAML::Value
        << bc2dComponent.restitution_threshold;

    out << YAML::EndMap;  // BoxCollider2DComponent
  }
  if (entity.HasComponent<CircleCollider2DComponent>()) {
    out << YAML::Key << "CircleCollider2DComponent";
    out << YAML::BeginMap;  // CircleCollider2DComponent

    auto& cc2dComponent = entity.GetComponent<CircleCollider2DComponent>();
    out << YAML::Key << "Offset" << YAML::Value << cc2dComponent.offset;
    out << YAML::Key << "Radius" << YAML::Value << cc2dComponent.radius;
    out << YAML::Key << "Density" << YAML::Value << cc2dComponent.density;
    out << YAML::Key << "Friction" << YAML::Value << cc2dComponent.friction;
    out << YAML::Key << "Restitution" << YAML::Value
        << cc2dComponent.restitution;
    out << YAML::Key << "RestitutionThreshold" << YAML::Value
        << cc2dComponent.restitution_threshold;

    out << YAML::EndMap;  // CircleCollider2DComponent
  }

  out << YAML::EndMap;  // Entity
}

void SceneSerializer::Serialize(const std::string& file_path) {
  YAML::Emitter out;
  out << YAML::BeginMap;
  out << YAML::Key << "Scene" << YAML::Value << "Untitled";
  out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
  scene_->registry_.each([&](auto entityID) {
    Entity entity = {entityID, scene_.get()};
    if (!entity) return;
    SerializeEntity(out, entity);
  });
  out << YAML::EndSeq;
  out << YAML::EndMap;

  std::ofstream fout(file_path);
  fout << out.c_str();
}

void SceneSerializer::SerializeRuntime(const std::string& file_path) {
  // Not implemented
  HM_CORE_ASSERT(false);
}

bool SceneSerializer::Deserialize(const std::string& file_path) {

  YAML::Node data;
  try {
    data = YAML::LoadFile(file_path);
  } catch (YAML::ParserException e) {
    return false;
  }

  if (!data["Scene"]) return false;
  std::string sceneName = data["Scene"].as<std::string>();
  HM_CORE_TRACE("Deserializing scene '{0}'", sceneName);

  auto entities = data["Entities"];
  if (entities) {
    for (auto entity : entities) {
      uint64_t uuid = entity["Entity"].as<uint64_t>();

      std::string name;
      auto tagComponent = entity["TagComponent"];
      if (tagComponent) name = tagComponent["Tag"].as<std::string>();

      HM_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid,
                    name);

      Entity deserializedEntity = scene_->CreateEntityWithUUID(uuid,name);

      auto transformComponent = entity["TransformComponent"];
      if (transformComponent) {
        // Entities always have transforms
        auto& tc = deserializedEntity.GetComponent<TransformComponent>();
        tc.translation = transformComponent["Translation"].as<glm::vec3>();
        tc.rotation = transformComponent["Rotation"].as<glm::vec3>();
        tc.scale = transformComponent["Scale"].as<glm::vec3>();
      }

      auto cameraComponent = entity["CameraComponent"];
      if (cameraComponent) {
        auto& cc = deserializedEntity.AddComponent<CameraComponent>();

        auto& cameraProps = cameraComponent["Camera"];
        cc.camera.SetProjectionType(
            (SceneCamera::ProjectionType)cameraProps["ProjectionType"]
                .as<int>());

        cc.camera.SetPerspectiveVerticalFOV(
            cameraProps["PerspectiveFOV"].as<float>());
        cc.camera.SetPerspectiveNearClip(
            cameraProps["PerspectiveNear"].as<float>());
        cc.camera.SetPerspectiveFarClip(
            cameraProps["PerspectiveFar"].as<float>());

        cc.camera.SetOrthographicSize(
            cameraProps["OrthographicSize"].as<float>());
        cc.camera.SetOrthographicNearClip(
            cameraProps["OrthographicNear"].as<float>());
        cc.camera.SetOrthographicFarClip(
            cameraProps["OrthographicFar"].as<float>());

        cc.primary = cameraComponent["Primary"].as<bool>();
        cc.fixed_aspect_ratio = cameraComponent["FixedAspectRatio"].as<bool>();
      }

      auto spriteRendererComponent = entity["SpriteRendererComponent"];
      if (spriteRendererComponent) {
        auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
        src.color = spriteRendererComponent["Color"].as<glm::vec4>();
      }

      auto circleRendererComponent = entity["CircleRendererComponent"];
      if (circleRendererComponent) {
        auto& crc = deserializedEntity.AddComponent<CircleRendererComponent>();
        crc.color = circleRendererComponent["Color"].as<glm::vec4>();
        crc.thickness = circleRendererComponent["Thickness"].as<float>();
        crc.fade = circleRendererComponent["Fade"].as<float>();
      }

      auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
      if (rigidbody2DComponent) {
        auto& rb2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
        rb2d.type = RigidBody2DBodyTypeFromString(
            rigidbody2DComponent["BodyType"].as<std::string>());
        rb2d.fixed_rotation = rigidbody2DComponent["FixedRotation"].as<bool>();
      }

      auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
      if (boxCollider2DComponent) {
        auto& bc2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
        bc2d.offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
        bc2d.size = boxCollider2DComponent["Size"].as<glm::vec2>();
        bc2d.density = boxCollider2DComponent["Density"].as<float>();
        bc2d.friction = boxCollider2DComponent["Friction"].as<float>();
        bc2d.restitution = boxCollider2DComponent["Restitution"].as<float>();
        bc2d.restitution_threshold =
            boxCollider2DComponent["RestitutionThreshold"].as<float>();
      }
      auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
      if (circleCollider2DComponent) {
        auto& cc2d =
            deserializedEntity.AddComponent<CircleCollider2DComponent>();
        cc2d.offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
        cc2d.radius = circleCollider2DComponent["Radius"].as<float>();
        cc2d.density = circleCollider2DComponent["Density"].as<float>();
        cc2d.friction = circleCollider2DComponent["Friction"].as<float>();
        cc2d.restitution = circleCollider2DComponent["Restitution"].as<float>();
        cc2d.restitution_threshold =
            circleCollider2DComponent["RestitutionThreshold"].as<float>();
      }
    }
  }
  return true;
}

bool SceneSerializer::DeserializeRuntime(const std::string& file_path) {
  // Not implemented
  HM_CORE_ASSERT(false);
  return false;
}

}  // namespace hammer