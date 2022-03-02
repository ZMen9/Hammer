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
  virtual void OnImGuiRender() override;


  void Begin();
  void End();

 private:
  float time_ = 0.0f;
};

}  // namespace hammer