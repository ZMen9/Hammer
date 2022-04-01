#include "hmpch.h"
#include <GLFW/glfw3.h>
#include "Hammer/Core/Application.h"
#include "Hammer/Renderer/Renderer.h"
#include "Hammer/Core/Input.h"
#include "Hammer/Core/Timestep.h"

namespace hammer {


Application* Application::instance_ = nullptr;

Application::Application() {
  HM_CORE_ASSERT(!instance_, "Application already exists!");
  HM_PROFILE_FUNCTION();
  instance_ = this;
  window_ = Window::Create();
  window_->SetEventCallback(HM_BIND_EVENT_FN(Application::OnEvent));

  Renderer::Init();

  ImGuiLayer_ = new ImGuiLayer();
  PushOverlay(ImGuiLayer_);
}

Application::~Application() {
  HM_PROFILE_FUNCTION();
  Renderer::Shutdown();
}


void Application::OnEvent(Event& e) {
  HM_PROFILE_FUNCTION();
  EventDispatcher dispatcher(e);
  dispatcher.Dispatch<WindowCloseEvent>(HM_BIND_EVENT_FN(Application::OnWindowClose));
  dispatcher.Dispatch<WindowResizeEvent>(HM_BIND_EVENT_FN(Application::OnWindowResize));

  for (auto it = layer_stack_.rbeing(); it != layer_stack_.rend();++it) {
    if (e.handled_) break;
    (*it)->OnEvent(e);
  }
}

void Application::Run() {
  HM_PROFILE_FUNCTION();
  while (running_) {
    HM_PROFILE_SCOPE("Runloop");
    float time = (float)glfwGetTime();
    Timestep ts = time - last_frame_time_;
    last_frame_time_ = time;

    if (!minimized_) {
      { 
        HM_PROFILE_SCOPE("LayerStack OnUpdate");
        for (Layer* layer : layer_stack_) {
          layer->OnUpdate(ts);
        }

      }

      ImGuiLayer_->Begin();

      {
        HM_PROFILE_SCOPE("LayerStack OnImGuiRender");
        for (Layer* layer : layer_stack_) {
          layer->OnImGuiRender();
        }
        
      }

      ImGuiLayer_->End();
    
    }
    window_->OnUpdate();
  }
}

bool Application::OnWindowClose(WindowCloseEvent& e) {
  running_ = false;
  return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e) {
  HM_PROFILE_FUNCTION();
  if (e.width() == 0 || e.height() == 0) {
    minimized_ = true;
    return false;
  }
  minimized_ = false;
  Renderer::OnWindowResize(e.width(), e.height());
  return false;
}

void Application::PushLayer(Layer* layer) { 
  HM_PROFILE_FUNCTION();
  layer_stack_.PushLayer(layer);
  layer->OnAttach();
}

void Application::PushOverlay(Layer* overlay) {
  HM_PROFILE_FUNCTION();
  layer_stack_.PushOverlay(overlay);
  overlay->OnAttach();
}

void Application::Close() { running_ = false; }

}  // namespace hammer
