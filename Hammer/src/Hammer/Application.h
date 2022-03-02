#pragma once
#include "Core.h"

#include "Window.h"
#include "Hammer/LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "Hammer/ImGui/ImGuiLayer.h"

namespace hammer {
class HAMMER_API Application {
 public:
  Application();
  virtual ~Application();
  void Run();
  void OnEvent(Event& e);
  bool OnWindowClose(WindowCloseEvent& e);

  void PushLayer(Layer* layer);
  void PushOverlay(Layer* overlay);

  inline static Application& instance() { return *instance_; }
  inline Window& window() { return *window_; }

 private: 
  std::unique_ptr<Window> window_;
  ImGuiLayer* ImGuiLayer_;
  bool running_ = true;
  LayerStack layer_stack_;

  static Application* instance_;
};

// Should be defind by client application
Application* CreateApplication();

}  // namespace Hammer
