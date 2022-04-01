#pragma once
#include "Hammer/Core/Base.h"

#include "Hammer/Core/Window.h"
#include "Hammer/Core/LayerStack.h"
#include "Hammer/Events/Event.h"
#include "Hammer/Events/ApplicationEvent.h"
#include "Hammer/Core/Timestep.h"

#include "Hammer/ImGui/ImGuiLayer.h"
// make main runloop only accessible on the engine side 
int main(int argc, char** argv);

namespace hammer {
class  Application {
 public:
  Application();
  virtual ~Application();
  void OnEvent(Event& e);

  void PushLayer(Layer* layer);
  void PushOverlay(Layer* overlay);

  static Application& instance() { return *instance_; }
  ImGuiLayer* GetImGuiLayer() { return ImGuiLayer_; }
  Window& window() { return *window_; }
  void Close();
  
 private: 
  void Run();

  bool OnWindowClose(WindowCloseEvent& e);
  bool OnWindowResize(WindowResizeEvent& e);

 private:
  std::unique_ptr<Window> window_;
  ImGuiLayer* ImGuiLayer_;
  bool running_ = true;
  bool minimized_ = false;
  LayerStack layer_stack_;
  float last_frame_time_ = 0.0f;
    
 private:
  static Application* instance_;
  friend int ::main(int argc, char** argv);
};

// Should be defined by client application
Application* CreateApplication();

}  // namespace Hammer
