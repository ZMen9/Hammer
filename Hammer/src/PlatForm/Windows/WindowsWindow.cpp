#include "hmpch.h"
#include "PlatForm/Windows/WindowsWindow.h"

#include "Hammer/Events/ApplicationEvent.h"
#include "Hammer/Events/KeyEvent.h"
#include "Hammer/Events/MouseEvent.h"
#include "PlatForm/OpenGL/OpenGLContext.h"
#include "Hammer/Renderer/Renderer.h"

namespace hammer {

static uint8_t kglfwInitialized = 0;

static void GLFWErrorCallback(int error_code, const char* description) {
  HM_CORE_ERROR("GLFW Error ( {0}): {1}", error_code, description);
}

WindowsWindow::WindowsWindow(const WindowProps& props) {
  HM_PROFILE_FUNCTION();
  Init(props);
}

WindowsWindow::~WindowsWindow() {
  HM_PROFILE_FUNCTION();
  Shutdown();
}

void WindowsWindow::Init(const WindowProps& props) {
  HM_PROFILE_FUNCTION();
  data_.title_ = props.title_;
  data_.width_ = props.width_;
  data_.height_ = props.height_;

  HM_CORE_INFO("Creating window {0} ({1}, {2})", props.title_, props.width_,
               props.height_);
  if (kglfwInitialized == 0) {
    HM_PROFILE_SCOPE("glfwInit");
    // TODO: glfwTerminate on system shutdown
    int success = glfwInit();
    HM_CORE_ASSERT(success, "Could not initialize GLFW!");

    glfwSetErrorCallback(GLFWErrorCallback);
  }

  SetMSAA(false);

  {
    HM_PROFILE_SCOPE("glfwCreateWindow");
    #if defined(HM_DEBUG)
    if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
      glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    #endif
    window_ = glfwCreateWindow((int)props.width_, (int)props.height_,
                               data_.title_.c_str(), nullptr, nullptr);
    ++kglfwInitialized;
  }
  context_ = GraphicsContext::Create(window_);
  context_->Init();

  glfwSetWindowUserPointer(window_, &data_);
  SetVSync(true);

  // Set GLFW callbacks
  glfwSetWindowSizeCallback(
      window_, [](GLFWwindow* window, int width, int height) {
        WindowData& data =
            *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        data.width_ = width;
        data.height_ = height;

        WindowResizeEvent event(width, height);
        data.EventCallback(event);
      });

  glfwSetWindowCloseCallback(window_, [](GLFWwindow* window) {
    WindowData& data =
        *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

    WindowCloseEvent event;
    data.EventCallback(event);
  });

  glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode,
                                 int action, int mods) {
    WindowData& data =
        *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
    switch (action) {
      case GLFW_PRESS: {
        KeyPressedEvent event(key, 0);
        data.EventCallback(event);
        break;
      }
      case GLFW_RELEASE: {
        KeyReleasedEvent event(key);
        data.EventCallback(event);
        break;
      }
      case GLFW_REPEAT: {
        KeyPressedEvent event(key, 1);
        data.EventCallback(event);
        break;
      }
    }
  });

  glfwSetCharCallback(window_, [](GLFWwindow* window, unsigned int key) {
    WindowData& data =
        *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

    KeyTypedEvent event(key);
    data.EventCallback(event);
  });

  glfwSetMouseButtonCallback(
      window_, [](GLFWwindow* window, int button, int action, int mods) {
        WindowData& data =
            *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        switch (action) {
          case GLFW_PRESS: {
            MouseButtonPressedEvent event(button);
            data.EventCallback(event);
            break;
          }
          case GLFW_RELEASE: {
            MouseButtonReleasedEvent event(button);
            data.EventCallback(event);
          }
        }
      });
  glfwSetScrollCallback(
      window_, [](GLFWwindow* window, double xoffset, double yoffset) {
        WindowData& data =
            *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        MouseScrolledEvent event(static_cast<float>(xoffset),
                                 static_cast<float>(yoffset));
        data.EventCallback(event);
      });

  glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double xpos,
                                       double ypos) {
    WindowData& data =
        *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
    MouseMovedEvent event(static_cast<float>(xpos), static_cast<float>(ypos));
    data.EventCallback(event);
  });
}

void WindowsWindow::OnUpdate() {
  HM_PROFILE_FUNCTION();
  glfwPollEvents();
  context_->SwapBuffers();
}

void WindowsWindow::SetVSync(bool enabled) {
  HM_PROFILE_FUNCTION();
  if (enabled)
    glfwSwapInterval(1);
  else
    glfwSwapInterval(0);
  data_.VSync_ = enabled;
}

bool WindowsWindow::IsVSync() const { return data_.VSync_; }


void WindowsWindow::Shutdown() { 
  HM_PROFILE_FUNCTION();
  glfwDestroyWindow(window_);
  --kglfwInitialized;
  if (kglfwInitialized == 0) {
    glfwTerminate();
  }


}

void WindowsWindow::SetMSAA(bool enabled) { 
  if(enabled)
    glfwWindowHint(GLFW_SAMPLES, 4); 
}

}  // namespace hammer
