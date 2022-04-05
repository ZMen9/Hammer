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
struct ApplicationCommandLineArgs {
  int count = 0;
  char** Args = nullptr;
  const char* operator[](int index) const {
    HM_CORE_ASSERT(index < count);
    return Args[index];
  }
};

class Application {
 public:
  Application(const std::string& name = "Hammer App",
              ApplicationCommandLineArgs args = ApplicationCommandLineArgs());

  virtual ~Application();
  void OnEvent(Event& e);

  void PushLayer(Layer* layer);
  void PushOverlay(Layer* overlay);

  static Application& instance() { return *instance_; }
  ApplicationCommandLineArgs GetCommandLineArgs() const {
    return commandline_args_;
  }
  ImGuiLayer* GetImGuiLayer() { return ImGuiLayer_; }
  Window& window() { return *window_; }
  void Close();

 private:
  void Run();

  bool OnWindowClose(WindowCloseEvent& e);
  bool OnWindowResize(WindowResizeEvent& e);

 private:
  Scope<Window> window_;
  ApplicationCommandLineArgs commandline_args_;
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
Application* CreateApplication(ApplicationCommandLineArgs);

}  // namespace Hammer

