#pragma once

#include <glm/glm.hpp>
#include "Hammer/Core/KeyCode.h"
#include "Hammer/Core/MouseCode.h"

namespace hammer {

class Input {

 public:
  
  static bool IsKeyPressed(KeyCode key);
  static bool IsMouseButtonPressed(MouseCode button);
  static glm::vec2 GetMousePosition();
  static float GetMouseX();
  static float GetMouseY();
};

}  // namespace hammer
