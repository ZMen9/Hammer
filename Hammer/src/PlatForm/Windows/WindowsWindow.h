#pragma once

#include "GLFW/glfw3.h"
#include "Window.h"

namespace hammer {
class WindowsWindow : public Window {
 public:
  WindowsWindow(const WindowProps& props); 
  virtual ~WindowsWindow();

  void OnUpdate() override;
  inline unsigned int width() const override { return data_.width_; }
  inline unsigned int height() const override { return data_.height_; }

  inline void SetEventCallback(const EventCallbackFn& callback) override {
    data_.EventCallback = callback;
  };
  void SetVSync(bool enabled) override;
  bool IsVSync() const override;
 private:
  virtual void Init(const WindowProps& props);
  virtual void Shutdown();

  GLFWwindow* window_;

  struct WindowData {
    std::string title_;
    unsigned int width_ = 0, height_ = 0;
    bool VSync_ = false;

    EventCallbackFn EventCallback;
  };

  WindowData data_;
};
}  // namespace hammer
