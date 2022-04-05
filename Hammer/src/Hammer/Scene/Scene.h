#pragma once
#include "entt.hpp"
#include "Hammer/Core/Timestep.h"
#include "Hammer/Renderer/EditorCamera.h"
#include "Hammer/Core/UUID.h"
class b2World;
namespace hammer {
class Entity;
class Scene {
 public:
  Scene();
  ~Scene();

  Entity CreateEntity(const std::string& name = std::string());
  // SceneSerializer::Deserialize will use this:
  Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
  void DestroyEntity(Entity entity);

  void OnRuntimeStart();
  void OnRuntimeStop();

  void OnUpdateRuntime(Timestep ts);
  void OnUpdateEditor(Timestep ts, EditorCamera& camera);
  void OnViewportResize(uint32_t width, uint32_t height);
  Entity GetPrimaryCameraEntity();

  static Ref<Scene> Copy(Ref<Scene> other);
  void DuplicateEntity(Entity entity);

  template <typename... Components>
  auto GetAllEntitiesWith() {
    return registry_.view<Components...>();
  }
 private:
  template <typename T>
  void OnComponentAdded(Entity entity, T& component);

 private:
  entt::registry registry_;
  uint32_t viewport_width_ = 0, viewport_height_ = 0;
  b2World* physics_world_= nullptr;
  friend class Entity;
  friend class SceneSerializer;
  friend class SceneHierarchyPanel;
};

} // namespace hammer