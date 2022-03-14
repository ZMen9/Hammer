#pragma once
#include <glm/glm.hpp>
#include "Hammer/Renderer/OrthographicCamera.h"
#include "Hammer/Events/MouseEvent.h"
#include "Hammer/Events/ApplicationEvent.h"
#include "Hammer/Core/Timestep.h"


namespace hammer {
class OrthographicCameraController {
 public:
  OrthographicCameraController(float aspect_ratio, bool rotation = false);

  void OnEvent(Event& e);
  void OnUpdate(Timestep ts);

  inline OrthographicCamera& GetCamera() { return camera_; }
  inline const OrthographicCamera& GetCamera() const { return camera_; }

 private:
  bool OnMouseScrolled(MouseScrolledEvent& e);
  bool OnWindowResized(WindowResizeEvent& e);
 private:
  float aspect_ratio_, zoom_level_ = 1.0f, camera_degrees_ = 0.0f;
  //camera initialization needs the variable "zoom_level",pay attention to the order
  OrthographicCamera camera_;
  float camera_translation_speed_ = 5.0f, camera_rotation_speed_ = 180.0f;
  bool rotation_;
  glm::vec3 camera_position_ = {0.0f, 0.0f, 0.0f};
};
} //namespace hammer