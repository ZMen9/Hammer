#pragma once
#include "Hammer/Core/Core.h"

#include "Hammer/Core/Window.h"
#include "Hammer/Core/LayerStack.h"
#include "Hammer/Events/Event.h"
#include "Hammer/Events/ApplicationEvent.h"

#include "Hammer/ImGui/ImGuiLayer.h"



namespace hammer {
class  Application {
 public:
  Application();
  virtual ~Application() = default;
  void Run();
  void OnEvent(Event& e);

  void PushLayer(Layer* layer);
  void PushOverlay(Layer* overlay);

  inline static Application& instance() { return *instance_; }
  inline Window& window() { return *window_; }

 private: 
  bool OnWindowClose(WindowCloseEvent& e);

 private:
  std::unique_ptr<Window> window_;
  ImGuiLayer* ImGuiLayer_;
  bool running_ = true;
  LayerStack layer_stack_;
  float last_frame_time_ = 0.0f;
    
 private:
  static Application* instance_;
};

// Should be defined by client application
Application* CreateApplication();

}  // namespace Hammer
