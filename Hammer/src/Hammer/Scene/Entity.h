#pragma once
#include "Hammer/Scene/Scene.h"
#include "entt.hpp"
#include "Hammer/Core/UUID.h"
#include "Hammer/Scene/Components.h"

namespace hammer {
class Entity {
 public:
  Entity() = default;
  Entity(entt::entity handle, Scene* scene);
  Entity(const Entity& other) = default;// 浅拷贝，但无需担心，很明显这样的行为是正确的

  template<typename T,typename... Args>
  T& AddComponent(Args&&... args) {
    HM_CORE_ASSERT(!HasComponent<T>(), "Entity already has the component!");
    T& component = scene_->registry_.emplace<T>(entity_handle_,
                                                std::forward<Args>(args)...);
    scene_->OnComponentAdded<T>(*this, component);
    return component;
  }
  template <typename T, typename... Args>
  T& AddOrReplaceComponent(Args&&... args) {
    T& component = scene_->registry_.emplace_or_replace<T>(entity_handle_,
                                                std::forward<Args>(args)...);
    scene_->OnComponentAdded<T>(*this, component);
    return component;
  }
  template<typename T>
  T& GetComponent() {
    HM_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
    return scene_->registry_.get<T>(entity_handle_);
  }
  template <typename T>
  bool HasComponent() {
    return scene_->registry_.all_of<T>(entity_handle_);
  }
  template <typename T>
  void RemoveComponent() {
    HM_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
    scene_->registry_.remove<T>(entity_handle_);
  }
  UUID GetUUID() { return GetComponent<IDComponent>().id; }
  const std::string& GetName() { return GetComponent<TagComponent>().tag; }
  operator bool() const { return entity_handle_ != entt::null; }
  operator entt::entity() const { return entity_handle_; }
  operator uint32_t() const {return static_cast<uint32_t>(entity_handle_);}
  bool operator==(const Entity& other) const {
    return entity_handle_ == other.entity_handle_ && scene_ == other.scene_;
  }
  bool operator!=(const Entity& other) { return !(*this == other); }
 private:
  entt::entity entity_handle_{entt::null};
  Scene* scene_ = nullptr;
};
} // namespace hammer
