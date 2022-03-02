#include "hmpch.h"
#include "WindowsInput.h"

#include <GLFW/glfw3.h>

#include "Hammer/Application.h"

namespace hammer {
Input* Input::kInstance = new WindowsInput();

bool WindowsInput::IsKeyPressedImpl(int keycode) {
  auto window = static_cast<GLFWwindow*>(
      Application::instance().window().GetNativeWindow());
  auto state = glfwGetKey(window, keycode);
  return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool WindowsInput::IsMouseButtonPressedImpl(int button) {
  auto window = static_cast<GLFWwindow*>(
      Application::instance().window().GetNativeWindow());
  auto state = glfwGetMouseButton(window, button);
  return state == GLFW_PRESS;
}

std::pair<float, float> WindowsInput::GetMousePositionImpl() {
  auto window = static_cast<GLFWwindow*>(
      Application::instance().window().GetNativeWindow());
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  return {(float)xpos, (float)ypos};
}

float WindowsInput::GetMouseXImpl() {
  auto [x, y] = GetMousePositionImpl();
  return x;
}

float WindowsInput::GetMouseYImpl() {
  auto [x, y] = GetMousePositionImpl();
  return y;
}

}  // namespace hammer