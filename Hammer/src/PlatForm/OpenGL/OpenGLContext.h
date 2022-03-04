#pragma once
#include "Hammer/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace hammer {

class OpenGLContext : public GraphicsContext {
 public:
  OpenGLContext(GLFWwindow* window_handle);
  void Init() override;
  void SwapBuffers() override;

 private:
  GLFWwindow* window_handle_; 
};
}