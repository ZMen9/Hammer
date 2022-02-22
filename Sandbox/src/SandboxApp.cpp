#include <Hammer.h>

class ExampleLayer : public hammer::Layer {
 public:
  ExampleLayer() : Layer("Example") {}

  void OnUpdate() override { HM_INFO("ExampleLayer::Update"); }

  void OnEvent(hammer::Event& event) override { HM_TRACE("{0}", event); }
};

class Sandbox : public hammer::Application {
 public:
  Sandbox() { PushLayer(new ExampleLayer()); }
  ~Sandbox(){};
};

hammer::Application* hammer::CreateApplication() { return new Sandbox(); }