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

  #ifdef HM_ENABLE_ASSERTS

  int version_major;
  int version_minor;
  glGetIntegerv(GL_MAJOR_VERSION, &version_major);
  glGetIntegerv(GL_MINOR_VERSION, &version_minor);
  HM_CORE_ASSERT(
      version_major > 4 || (version_major == 4 && version_minor >= 5),
      "Hammer requires at least OpenGL version 4.5!")

  #endif  // HM_ENABLE_ASSERTS
}

void OpenGLContext::SwapBuffers() {
  HM_PROFILE_FUNCTION();
  glfwSwapBuffers(window_handle_);
}

}  // namespace hammer