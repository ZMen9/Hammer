#include "hmpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_opengl3_loader.h"

#include "Hammer/Application.h"


// TEMPORARY
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace hammer {

ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

ImGuiLayer::~ImGuiLayer() {}

void ImGuiLayer::OnAttach() {
  // Setup Dear ImGui context IMGUI_CHECKVERSION();
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad
  // Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport
                                                       // / Platform Windows
  // io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
  // io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

  // TEMPORARY: should eventually use Hammer key codes
  io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB; 
  io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
  io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
  io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
  io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
  io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
  io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
  io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
  io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
  io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
  io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
  io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
  io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
  io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
  io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
  io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
  io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
  io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
  io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
  io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
  io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsClassic();

  // Setup Platform/Renderer backends
  // ImGui_ImplGlfw_InitForOpenGL(window, true);
  const char* glsl_version = "#version 410 core";
  ImGui_ImplOpenGL3_Init(glsl_version);
}

void ImGuiLayer::OnDetach() {}

void ImGuiLayer::OnEvent(Event& event) {
  EventDispatcher dispatcher(event);
  dispatcher.Dispatch<MouseButtonPressedEvent>(
      HM_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonPressedEvent));
  dispatcher.Dispatch<MouseButtonReleasedEvent>(
      HM_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonReleasedEvent));
  dispatcher.Dispatch<MouseMovedEvent>(
      HM_BIND_EVENT_FN(ImGuiLayer::OnMouseMovedEvent));
  dispatcher.Dispatch<MouseScrolledEvent>(
      HM_BIND_EVENT_FN(ImGuiLayer::OnMouseScrolledEvent));
  dispatcher.Dispatch<KeyPressedEvent>(
      HM_BIND_EVENT_FN(ImGuiLayer::OnKeyPressedEvent));
  dispatcher.Dispatch<KeyReleasedEvent>(
      HM_BIND_EVENT_FN(ImGuiLayer::OnKeyReleasedEvent));
  dispatcher.Dispatch<KeyTypedEvent>(
      HM_BIND_EVENT_FN(ImGuiLayer::OnKeyTypedEvent));
  dispatcher.Dispatch<WindowResizeEvent>(
      HM_BIND_EVENT_FN(ImGuiLayer::OnWindowResizeEvent));
}

void ImGuiLayer::OnUpdate() {
  ImGuiIO& io = ImGui::GetIO();
  Application& app = Application::instance();
  io.DisplaySize =
      ImVec2((float)app.window().width(), (float)app.window().height());

  float cur_time = static_cast<float>(glfwGetTime());
  io.DeltaTime = time_ > 0.0f ? (cur_time - time_) : (1.0f / 60.0f);
  time_ = cur_time;

  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();

  static bool show = true;
  ImGui::ShowDemoWindow(&show);

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.MouseDown[e.button()] = true;

  return false;
}

bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.MouseDown[e.button()] = false;

  return false;
}
bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.MousePos = ImVec2(e.mouse_x(), e.mouse_y());

  return false;
}

bool ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.MouseWheelH += e.offset_x();
  io.MouseWheel += e.offset_y();

  return false;
}

bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e) { 
  ImGuiIO& io = ImGui::GetIO();
  io.KeysDown[e.keycode()] = true;

  io.KeyCtrl =
      io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
  io.KeyShift =
      io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
  io.KeyAlt =
      io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
  io.KeySuper =
      io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];


  return false;
}

bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent& e) { 
  ImGuiIO& io = ImGui::GetIO();
  io.KeysDown[e.keycode()] = false;

  return false;
}

bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  int key_code = e.keycode();
  if (key_code > 0 && key_code < 0x10000)
    io.AddInputCharacter((unsigned int)key_code);

  return false;
}

bool ImGuiLayer::OnWindowResizeEvent(WindowResizeEvent& e) { 
  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize = ImVec2(e.width(), e.height());
  io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
  glViewport(0, 0, e.width(), e.height());

  return false;

}


}  // namespace hammer
