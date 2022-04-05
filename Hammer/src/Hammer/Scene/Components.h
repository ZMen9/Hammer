#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define  GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "Hammer/Scene/SceneCamera.h"
#include "Hammer/Renderer/Texture.h"
#include "Hammer/Core/UUID.h"

namespace hammer {
struct IDComponent {
  UUID id;
  IDComponent() = default;
  IDComponent(const IDComponent&) = default;
};

struct TagComponent {
  std::string tag;
  TagComponent() = default;
  TagComponent(const std::string& tag) : tag(tag) {}
  TagComponent(const TagComponent&) = default;
};
struct TransformComponent {
  glm::vec3 translation = {0.0f, 0.0f, 0.0f};
  glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
  glm::vec3 scale = {1.0f,1.0f,1.0f};
  TransformComponent() = default;
  TransformComponent(const glm::vec3& translation) : translation(translation) {}
  TransformComponent(const TransformComponent&) = default;

  glm::mat4 GetTransform() const {
    glm::mat4 rt = glm::toMat4(glm::quat(rotation));
    return glm::translate(glm::mat4(1.0f), translation) * rt *
           glm::scale(glm::mat4(1.0f), scale);
  }
};

struct SpriteRendererComponent {
  glm::vec4 color {1.0f, 1.0f, 1.0f, 1.0f};
  Ref<Texture2D> texture;
  float tiling_factor = 1.0f;
  SpriteRendererComponent() = default;
  SpriteRendererComponent(const SpriteRendererComponent&) = default;
  SpriteRendererComponent(const glm::vec4& color) : color(color) {}

};

struct CircleRendererComponent {
  glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
  float thickness = 1.0f;
  float fade = 0.005f;

  CircleRendererComponent() = default;
  CircleRendererComponent(const CircleRendererComponent&) = default;
};

class ScriptableEntity;
struct NativeScriptComponent {
  ScriptableEntity* instance = nullptr;
  ScriptableEntity* (*InstantiateScript)();
  void (*DestroyScript)(NativeScriptComponent*);

  template<typename T>
  void Bind() {
    InstantiateScript = []() {
      return static_cast<ScriptableEntity*>(new T());
    };
    DestroyScript = [](NativeScriptComponent* nsc) {
      delete nsc->instance;
      nsc->instance = nullptr;
    };
  }
};

struct CameraComponent {
  SceneCamera camera;
  bool primary = true;  // TODO: think about moving to Scene
  bool fixed_aspect_ratio = false;
  CameraComponent() = default;
  CameraComponent(const CameraComponent&) = default;
};

struct Rigidbody2DComponent {
  enum class BodyType {
    STATIC = 0,
    DYNAMIC ,
    KINEMATIC ,
  };
  // Storage for runtime
  BodyType type = BodyType::STATIC;
  bool fixed_rotation = false;
  void* runtime_body = nullptr;
  Rigidbody2DComponent() = default;
  Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
};
struct BoxCollider2DComponent {
  glm::vec2 offset = {0.0f, 0.0f};
  glm::vec2 size = {0.5f, 0.5f};

  // TODO: move into physics material in the future maybe
  float density = 1.0f;
  float friction = 0.5f;
  float restitution = 0.0f;
  float restitution_threshold = 0.5f;
  // Storage for runtime
  void* runtime_fixture = nullptr;
  BoxCollider2DComponent() = default;
  BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
};

struct CircleCollider2DComponent {
  glm::vec2 offset = {0.0f, 0.0f};
  float radius = 0.5f;

  // TODO: move into physics material in the future maybe
  float density = 1.0f;
  float friction = 0.5f;
  float restitution = 0.0f;
  float restitution_threshold = 0.5f;

  // Storage for runtime
  void* runtime_fixture = nullptr;

  CircleCollider2DComponent() = default;
  CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
};
} // namespace hammer