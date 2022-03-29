#pragma once

#include "Hammer/Core/Window.h"
#include "Hammer/Renderer/GraphicsContext.h"
#include <GLFW/glfw3.h>

namespace hammer {
class WindowsWindow : public Window {
 public:
  WindowsWindow(const WindowProps& props); 
  virtual ~WindowsWindow();


  void OnUpdate() override;
  inline unsigned int width() const override { return data_.width_; }
  inline unsigned int height() const override { return data_.height_; }

  void SetEventCallback(const EventCallbackFn& callback) override {
    data_.EventCallback = callback;
  };
  void SetVSync(bool enabled) override;
  bool IsVSync() const override;

  
  inline virtual void* GetNativeWindow() const override { return window_; }
 private:
  virtual void Init(const WindowProps& props);
  virtual void Shutdown();
  void SetMSAA(bool enabled);

 private:
  GLFWwindow* window_;
  Scope<GraphicsContext> context_;

  struct WindowData {
    std::string title_;
    unsigned int width_ = 0, height_ = 0;
    bool VSync_ = false;

    EventCallbackFn EventCallback;
  };

  WindowData data_;
};
}  // namespace hammer
