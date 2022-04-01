#pragma once

#include <sstream>
#include "Hammer/Core/Base.h"
#include "Hammer/Events/Event.h"

namespace hammer {
struct WindowProps {
  std::string title_;
  unsigned int width_;
  unsigned int height_;

  WindowProps(const std::string& title = "Hammer Engine",
              unsigned int width = 1600, unsigned int height = 900)
      : title_(title), width_(width), height_(height) {}
};

// Interface representing a desktop system based Window 
class Window {
 public:
  using EventCallbackFn = std::function<void(Event&)>;

  virtual ~Window() = default;
  virtual void OnUpdate() = 0;
  virtual unsigned int width() const = 0;
  virtual unsigned int height() const = 0;

  virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
  virtual void SetVSync(bool enable) = 0;
  virtual bool IsVSync() const = 0;

  virtual void* GetNativeWindow() const = 0;

  static Scope<Window>Create(const WindowProps& props = WindowProps());
};

}  // namespace hammer
