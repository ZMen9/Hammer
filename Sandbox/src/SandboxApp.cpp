#include <Hammer.h>

class ExampleLayer : public hammer::Layer {
 public:
  ExampleLayer() : Layer("Example") {}

  void OnUpdate() override {
    if (hammer::Input::IsKeyPressed(HM_KEY_TAB)) 
      HM_TRACE("Key tab is pressed!");
  }

  void OnEvent(hammer::Event& event) override { 
    if (event.GetEventType() == hammer::EventType::kKeyPressed) {
      hammer::KeyPressedEvent& e = static_cast<hammer::KeyPressedEvent&>(event);
      HM_TRACE("{0}", (char)e.keycode());
    }
    
  }
};

class Sandbox : public hammer::Application {
 public:
  Sandbox() { 
    PushLayer(new ExampleLayer());
    PushOverlay(new hammer::ImGuiLayer());
  }
  ~Sandbox(){};
};

hammer::Application* hammer::CreateApplication() { return new Sandbox(); }