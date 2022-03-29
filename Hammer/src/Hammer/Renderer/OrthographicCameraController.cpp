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
  if (Input::IsKeyPressed(HM_KEY_A)) {
    camera_position_.x -= cos(glm::radians(camera_degrees_)) * camera_translation_speed_ * ts;
    camera_position_.y -=
        sin(glm::radians(camera_degrees_)) * camera_translation_speed_ * ts;
  }
  else if (Input::IsKeyPressed(HM_KEY_D)) {
    camera_position_.x +=
        cos(glm::radians(camera_degrees_)) * camera_translation_speed_ * ts;
    camera_position_.y +=
        sin(glm::radians(camera_degrees_)) * camera_translation_speed_ * ts;
  }
  if (Input::IsKeyPressed(HM_KEY_W)) {
    camera_position_.x +=
        -sin(glm::radians(camera_degrees_)) * camera_translation_speed_ * ts;
    camera_position_.y +=
        cos(glm::radians(camera_degrees_)) * camera_translation_speed_ * ts;
  }
  else if (Input::IsKeyPressed(HM_KEY_S)) {
    camera_position_.x -=
        -sin(glm::radians(camera_degrees_)) * camera_translation_speed_ * ts;
    camera_position_.y -=
        cos(glm::radians(camera_degrees_)) * camera_translation_speed_ * ts;
  }

  camera_.set_position(camera_position_);
  camera_translation_speed_ = zoom_level_;

  if (rotation_) {
    if (Input::IsKeyPressed(HM_KEY_Q))
      camera_degrees_ += camera_rotation_speed_ * ts;
    if (Input::IsKeyPressed(HM_KEY_E))
      camera_degrees_ -= camera_rotation_speed_ * ts;
    if (camera_degrees_ > 180.0f) {
      camera_degrees_ -= 360.0f;
    } else if (camera_degrees_ <= -180.0f) {
      camera_degrees_ += 360.0f;
    }

    camera_.set_degrees(camera_degrees_);
  }
}

bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e) {
  HM_PROFILE_FUNCTION();
  zoom_level_ -= e.offset_y() * 0.25f;
  zoom_level_ = std::max(zoom_level_, 0.25f);
  bounds_ = {-aspect_ratio_ * zoom_level_, aspect_ratio_ * zoom_level_,
           -zoom_level_, zoom_level_};
  camera_.set_projection(bounds_.left, bounds_.right, bounds_.bottom, bounds_.top);
  return false;
}

bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e) {
  HM_PROFILE_FUNCTION();
  aspect_ratio_ = (float)e.width() / (float)e.height();
  bounds_ = {-aspect_ratio_ * zoom_level_, aspect_ratio_ * zoom_level_,
             -zoom_level_, zoom_level_};
  camera_.set_projection(bounds_.left, bounds_.right, bounds_.bottom,
                         bounds_.top);
  return false;
}

}  // namespace hammer