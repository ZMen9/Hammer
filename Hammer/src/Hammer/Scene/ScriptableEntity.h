#pragma once
#include "Hammer/Scene/Entity.h"

namespace hammer {
class ScriptableEntity {
 public:
  virtual ~ScriptableEntity() = default;
  template <typename T>
  T& GetComponent() {
    return entity_.GetComponent<T>();
  }

 protected:
  virtual void OnCreate() {}
  virtual void OnDestroy() {}
  virtual void OnUpdate(Timestep ts) {}
 private:
  Entity entity_;
  friend class Scene;
};

}  // namespace hammer