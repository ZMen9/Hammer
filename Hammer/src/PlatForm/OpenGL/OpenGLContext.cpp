#include "hmpch.h"
#include "PlatForm/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>

namespace hammer {

OpenGLContext::OpenGLContext(GLFWwindow* window_handle) 
		: window_handle_(window_handle) {
  HM_CORE_ASSERT(window_handle, "Window handle is null!")
}

void OpenGLContext::Init() {
  HM_PROFILE_FUNCTION();
  glfwMakeContextCurrent(window_handle_);
  int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  HM_CORE_ASSERT(status, "Failed to initialize Glad!");

  HM_CORE_INFO("OpenGL Info:");
  HM_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
  HM_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
  HM_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));
	
	HM_CORE_ASSERT(
      GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5),
      "Hazel requires at least OpenGL version 4.5!");
}

void OpenGLContext::SwapBuffers() {
  HM_PROFILE_FUNCTION();
  glfwSwapBuffers(window_handle_);
}

}  // namespace hammer
