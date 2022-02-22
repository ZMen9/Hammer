#pragma once
#include "Hammer/Layer.h"
#include "Hammer/Events/Event.h"
namespace hammer {
class ImGuiLayer : public Layer {
 public:
  ImGuiLayer();
  ~ImGuiLayer();

  virtual void OnAttach() override;
  virtual void OnDetach() override;
  void OnEvent(Event& event) override;
  void OnUpdate() override;
 private:
};

}  // namespace hammer