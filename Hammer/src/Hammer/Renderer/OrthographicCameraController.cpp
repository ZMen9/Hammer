#include "hmpch.h"
#include "Hammer/Renderer/OrthographicCameraController.h"
#include "Hammer/Core/Input.h"
#include "Hammer/Core/KeyCode.h"

namespace hammer {

OrthographicCameraController::OrthographicCameraController(
    float aspect_ratio, bool rotation /*= false*/) 
    : aspect_ratio_(aspect_ratio), 
      bounds_({-aspect_ratio_ * zoom_level_, aspect_ratio_ * zoom_level_,
               -zoom_level_, zoom_level_}),
      camera_(bounds_.left,bounds_.right,bounds_.bottom, bounds_.top),
      rotation_(rotation) {}

void OrthographicCameraController::OnEvent(Event& e) {
  HM_PROFILE_FUNCTION();
  EventDispatcher dispatcher(e);
  dispatcher.Dispatch<MouseScrolledEvent>(HM_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
  dispatcher.Dispatch<WindowResizeEvent>(HM_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
}

void OrthographicCameraController::OnUpdate(Timestep ts) {
  HM_PROFILE_FUNCTION();
  if (Input::IsKeyPressed(key::A)) {
    camera_position_.x -= cos(glm::radians(camera_degrees_)) * camera_translation_speed_ * ts;
    camera_position_.y -=
        sin(glm::radians(camera_degrees_)) * camera_translation_speed_ * ts;
  }
  else if (Input::IsKeyPressed(key::D)) {
    camera_position_.x +=
        cos(glm::radians(camera_degrees_)) * camera_translation_speed_ * ts;
    camera_position_.y +=
        sin(glm::radians(camera_degrees_)) * camera_translation_speed_ * ts;
  }
  if (Input::IsKeyPressed(key::W)) {
    camera_position_.x +=
        -sin(glm::radians(camera_degrees_)) * camera_translation_speed_ * ts;
    camera_position_.y +=
        cos(glm::radians(camera_degrees_)) * camera_translation_speed_ * ts;
  }
  else if (Input::IsKeyPressed(key::S)) {
    camera_position_.x -=
        -sin(glm::radians(camera_degrees_)) * camera_translation_speed_ * ts;
    camera_position_.y -=
        cos(glm::radians(camera_degrees_)) * camera_translation_speed_ * ts;
  }

  camera_.set_position(camera_position_);
  camera_translation_speed_ = zoom_level_;

  if (rotation_) {
    if (Input::IsKeyPressed(key::Q))
      camera_degrees_ += camera_rotation_speed_ * ts;
    if (Input::IsKeyPressed(key::E))
      camera_degrees_ -= camera_rotation_speed_ * ts;
    if (camera_degrees_ > 180.0f) {
      camera_degrees_ -= 360.0f;
    } else if (camera_degrees_ <= -180.0f) {
      camera_degrees_ += 360.0f;
    }

    camera_.set_degrees(camera_degrees_);
  }
}

void OrthographicCameraController::OnResize(float width, float height) {
  aspect_ratio_ = width / height;
  camera_.set_projection(-aspect_ratio_ * zoom_level_,
                         aspect_ratio_ * zoom_level_, -zoom_level_,
                         zoom_level_);
}

void OrthographicCameraController::CalculateView() {
  bounds_ = {-aspect_ratio_ * zoom_level_, aspect_ratio_ * zoom_level_,
             -zoom_level_, zoom_level_};
  camera_.set_projection(bounds_.left, bounds_.right, bounds_.bottom,
                         bounds_.top);
}

bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e) {
  HM_PROFILE_FUNCTION();
  zoom_level_ -= e.offset_y() * 0.25f;
  zoom_level_ = std::max(zoom_level_, 0.25f);
  CalculateView();
  return false;
}

bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e) {
  HM_PROFILE_FUNCTION();
  OnResize((float)e.width(), (float)e.height());
  return false;
}


}  // namespace hammer
