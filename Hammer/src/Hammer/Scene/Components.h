#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Hammer/Scene/SceneCamera.h"
#include "Hammer/Scene/ScriptableEntity.h"

namespace hammer {
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
    glm::mat4 rt = glm::rotate(glm::mat4(1.0f), rotation.x, {1, 0, 0}) *
                   glm::rotate(glm::mat4(1.0f), rotation.y, {0, 1, 0}) *
                   glm::rotate(glm::mat4(1.0f), rotation.z, {0, 0, 1});
    return glm::translate(glm::mat4(1.0f), translation) * rt *
           glm::scale(glm::mat4(1.0f), scale);
  }
  
};

struct SpriteRendererComponent {
  glm::vec4 color {1.0f, 1.0f, 1.0f, 1.0f};
  SpriteRendererComponent() = default;
  SpriteRendererComponent(const SpriteRendererComponent&) = default;
  SpriteRendererComponent(const glm::vec4& color) : color(color) {}

};

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
} // namespace hammer