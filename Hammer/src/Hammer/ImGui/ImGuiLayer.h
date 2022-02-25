#pragma once
#include "Hammer/Layer.h"
#include "Hammer/Events/ApplicationEvent.h"
#include "Hammer/Events/KeyEvent.h"
#include "Hammer/Events/MouseEvent.h"
namespace hammer {
class HAMMER_API ImGuiLayer : public Layer {
 public:
  ImGuiLayer();
  ~ImGuiLayer();

  virtual void OnAttach() override;
  virtual void OnDetach() override;
  void OnEvent(Event& event) override;
  void OnUpdate() override;

 private:
  bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
  bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
  bool OnMouseMovedEvent(MouseMovedEvent& e);
  bool OnMouseScrolledEvent(MouseScrolledEvent& e);
  bool OnKeyPressedEvent(KeyPressedEvent& e);
  bool OnKeyReleasedEvent(KeyReleasedEvent& e);
  bool OnKeyTypedEvent(KeyTypedEvent& e);
  bool OnWindowResizeEvent(WindowResizeEvent& e);

 private:
  float time_ = 0.0f;
};

}  // namespace hammer