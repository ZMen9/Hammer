#pragma once
#include "Core.h"

#include "Window.h"
#include "Hammer/LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "Hammer/ImGui/ImGuiLayer.h"
#include "Hammer/Renderer/Shader.h"

#include "Hammer/Renderer/Buffer.h"

namespace hammer {
class HAMMER_API Application {
 public:
  Application();
  virtual ~Application() = default;
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

  unsigned int vertex_array_;
  std::unique_ptr<Shader> shader_;
  std::unique_ptr<VertexBuffer> vertex_buffer_; 
  std::unique_ptr<IndexBuffer> index_buffer_;

 private:
  static Application* instance_;
};

// Should be defined by client application
Application* CreateApplication();

}  // namespace Hammer
