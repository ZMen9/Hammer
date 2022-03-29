#include <Hammer.h>
#include <Hammer/Core/EntryPoint.h>

#include "PlatForm/OpenGL/OpenGLShader.h"
#include "Sandbox2D.h"
#include "ExampleLayer.h"

class Sandbox : public hammer::Application {
 public:
  Sandbox() {
    // PushLayer(new ExampleLayer());
    PushLayer(new Sandbox2D());
  }
  ~Sandbox(){};
};

hammer::Application* hammer::CreateApplication() { return new Sandbox(); }