#include "hmpch.h"
#include <GLFW/glfw3.h>
#include "Application.h"
#include "Hammer/Renderer/Renderer.h"
#include "Hammer/Core/Input.h"
#include "Hammer/Core/Timestep.h"

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
namespace hammer {

Application* Application::instance_ = nullptr;

Application::Application() {
  HM_CORE_ASSERT(!instance_, "Application already exists!");
  instance_ = this;
  window_ = std::unique_ptr<Window>(Window::Create());
  window_->SetEventCallback(BIND_EVENT_FN(OnEvent));

  Renderer::Init();

  ImGuiLayer_ = new ImGuiLayer();
  PushOverlay(ImGuiLayer_);
}


void Application::OnEvent(Event& e) {
  EventDispatcher dispatcher(e);
  dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

  for (auto it = layer_stack_.end(); it != layer_stack_.begin();) {
    (*--it)->OnEvent(e);
    if (e.handled_) break;
  }
}

void Application::Run() {
  while (running_) {
    float time = (float)glfwGetTime();
    Timestep ts = time - last_frame_time_;
    last_frame_time_ = time;
    for (Layer* layer : layer_stack_) {
      layer->OnUpdate(ts);
    }

    ImGuiLayer_->Begin();
    for (Layer* layer : layer_stack_) {
      layer->OnImGuiRender();
    }
    ImGuiLayer_->End();
    window_->OnUpdate();
  }
}

bool Application::OnWindowClose(WindowCloseEvent& e) {
  running_ = false;
  return true;
}

void Application::PushLayer(Layer* layer) { 
  layer_stack_.PushLayer(layer);
}

void Application::PushOverlay(Layer* overlay) {
  layer_stack_.PushOverlay(overlay);
}

}  // namespace hammer
