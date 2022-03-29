#pragma once
#include "Hammer/Core/Layer.h"
#include "Hammer/Events/ApplicationEvent.h"
#include "Hammer/Events/KeyEvent.h"
#include "Hammer/Events/MouseEvent.h"
namespace hammer {
class ImGuiLayer : public Layer {
 public:
  ImGuiLayer();
  ~ImGuiLayer() = default;

  virtual void OnAttach() override;
  virtual void OnDetach() override;
  virtual void OnEvent(Event& e) override;


  void Begin();
  void End();

 private:
  float time_ = 0.0f;
};

}  // namespace hammer