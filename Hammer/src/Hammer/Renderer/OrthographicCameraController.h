#pragma once

#include "Hammer/Renderer/OrthographicCamera.h"
#include "Hammer/Events/MouseEvent.h"
#include "Hammer/Events/ApplicationEvent.h"
#include "Hammer/Core/Timestep.h"

namespace hammer {

struct OrthographicCameraBounds {
  float left, right;
  float bottom, top;

  float GetWidth() { return right - left; }
  float GetHeight() { return top - bottom; }
};

class OrthographicCameraController {
 public:
  OrthographicCameraController(float aspect_ratio, bool rotation = true);

  void OnEvent(Event& e);
  void OnUpdate(Timestep ts);
  void OnResize(float width, float height);

  OrthographicCamera& GetCamera() { return camera_; }
  const OrthographicCamera& GetCamera() const { return camera_; }

  void set_zoom_level(float level) {
    zoom_level_ = level;
    CalculateView();
  }
  const float zoom_level() const { return zoom_level_; }
  const OrthographicCameraBounds& GetBounds() const { return bounds_; }


 private:
  void CalculateView();
  bool OnMouseScrolled(MouseScrolledEvent& e);
  bool OnWindowResized(WindowResizeEvent& e);
 private:
  float aspect_ratio_, zoom_level_ = 1.0f, camera_degrees_ = 0.0f;
  //camera initialization needs the variable "zoom_level",pay attention to the order
  OrthographicCameraBounds bounds_;// not store in class camera
  OrthographicCamera camera_;
  float camera_translation_speed_ = 5.0f, camera_rotation_speed_ = 180.0f;
  bool rotation_;
  glm::vec3 camera_position_ = {0.0f, 0.0f, 0.0f};
};
} //namespace hammer