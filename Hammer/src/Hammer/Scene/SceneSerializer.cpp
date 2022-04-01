#include "hmpch.h"
#include "Hammer/Scene/SceneSerializer.h"
#include <fstream>
#include <yaml-cpp/yaml.h>
#include "Hammer/Scene/Entity.h"
#include "Hammer/Scene/Components.h"
namespace YAML {
template<>
struct convert<glm::vec3> {
  static Node encode(const glm::vec3& rhs) {
    Node node;
    node.push_back(rhs.x);
    node.push_back(rhs.y);
    node.push_back(rhs.z);
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

SceneSerializer::SceneSerializer(const Ref<Scene> scene) 
		:scene_(scene) {}

// universally unique identifiers:uuids
static void SerializeEntity(YAML::Emitter& out, Entity entity) {
  out << YAML::BeginMap;  // Entity
  out << YAML::Key << "Entity" << YAML::Value
      << "12837192831273";  // TODO: this is just a random number, 
                            // true Entity ID will goes here

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
  std::ifstream stream(file_path);
  std::stringstream strStream;
  strStream << stream.rdbuf();

  YAML::Node data = YAML::Load(strStream.str());
  if (!data["Scene"]) return false;

  std::string sceneName = data["Scene"].as<std::string>();
  HM_CORE_TRACE("Deserializing scene '{0}'", sceneName);

  auto entities = data["Entities"];
  if (entities) {
    for (auto entity : entities) {
      uint64_t uuid = entity["Entity"].as<uint64_t>();  // TODO

      std::string name;
      auto tagComponent = entity["TagComponent"];
      if (tagComponent) name = tagComponent["Tag"].as<std::string>();

      HM_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid,
                    name);

      Entity deserializedEntity = scene_->CreateEntity(name);

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