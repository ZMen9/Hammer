#pragma once
#include "entt.hpp"
#include "Hammer/Core/Timestep.h"
namespace hammer {
class Entity;
class Scene {
 public:
  Scene();
  ~Scene();

  Entity CreateEntity(const std::string& name = std::string());
  void DestroyEntity(Entity entity);
  void OnUpdate(Timestep ts);
  void OnViewportResize(uint32_t width, uint32_t height);

 private:
  template <typename T>
  void OnComponentAdded(Entity entity, T& component);
 private:
  entt::registry registry_;
  uint32_t viewport_width_ = 0, viewport_height_ = 0;
  friend class Entity;
  friend class SceneSerializer;
  friend class SceneHierarchyPanel;
};

} // namespace hammer