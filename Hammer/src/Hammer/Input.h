#pragma once

#include "Hammer/Core.h"

namespace hammer {

class HAMMER_API Input {
 public:
  inline static bool IsKeyPressed(int keycode) {
    return kInstance->IsKeyPressedImpl(keycode);
  }

  inline static bool IsMouseButtonPressed(int button) {
    return kInstance->IsMouseButtonPressedImpl(button);
  }
  inline static std::pair<float, float> GetMousePosition() {
    return kInstance->GetMousePositionImpl();
  }
  inline static float GetMouseX() { return kInstance->GetMouseXImpl(); }
  inline static float GetMouseY() { return kInstance->GetMouseYImpl(); }

 protected:
  virtual bool IsKeyPressedImpl(int keycode) = 0;
  virtual bool IsMouseButtonPressedImpl(int button) = 0;
  virtual std::pair<float, float> GetMousePositionImpl() = 0;
  virtual float GetMouseXImpl() = 0;
  virtual float GetMouseYImpl() = 0;

 private:
  static Input* kInstance;
};

}  // namespace hammer