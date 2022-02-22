#include "hmpch.h"
#include "Application.h"
#include <Glad/glad.h>


#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
namespace hammer {
Application::Application() {
  window_ = std::unique_ptr<Window>(Window::Create());
  window_->SetEventCallback(BIND_EVENT_FN(OnEvent));

  
}

Application::~Application() {}

void Application::OnEvent(Event& e) {
  EventDispatcher dispatcher(e);
  dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

  for (auto it = layer_stack_.end(); it != layer_stack_.begin();) {
    (*--it)->OnEvent(e);
    if (e.handled_)
      break;
  }
}

void Application::Run() {
  while (running_) {
    glClearColor(1, 0, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    for (Layer* layer : layer_stack_) {
      layer->OnUpdate();
    }
    window_->OnUpdate();
  }
}

bool Application::OnWindowClose(WindowCloseEvent& e) {
  running_ = false;
  return true;
}

void Application::PushLayer(Layer* layer) { layer_stack_.PushLayer(layer); }

void Application::PushOverlay(Layer* overlay) {
  layer_stack_.PushOverlay(overlay);
}

}  // namespace hammer
