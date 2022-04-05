#include <Hammer.h>
#include <Hammer/Core/EntryPoint.h>

#include "PlatForm/OpenGL/OpenGLShader.h"
#include "Sandbox2D.h"
#include "ExampleLayer.h"

class Sandbox : public hammer::Application {
 public:
  Sandbox(hammer::ApplicationCommandLineArgs args) {
    // PushLayer(new ExampleLayer());
    PushLayer(new Sandbox2D());
  }
  ~Sandbox(){};
};

hammer::Application* hammer::CreateApplication(hammer::ApplicationCommandLineArgs args) {
  return new Sandbox(args);
}