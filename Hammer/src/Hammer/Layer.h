#pragma once

#include "Hammer/Core.h"
#include "Hammer/Events/Event.h"
#include "Hammer/Core/Timestep.h"
namespace hammer {
class HAMMER_API Layer {
 public:
  Layer(const std::string& name = "Layer");
  virtual ~Layer() = default;

  virtual void OnAttach() {}
  virtual void OnDetach() {}
  virtual void OnUpdate(Timestep ts) {}
  virtual void OnImGuiRender() {}
  virtual void OnEvent(Event& event) {}

  inline const std::string& debug_name() const { return debug_name_; }
 private: 
  std::string debug_name_;
};
}  // namespace hammer