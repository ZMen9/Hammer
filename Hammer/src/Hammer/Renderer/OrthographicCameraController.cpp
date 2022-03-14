#include "hmpch.h"
#include "OrthographicCameraController.h"
#include "Hammer/Core/Input.h"
#include "Hammer/Core/KeyCode.h"

namespace hammer {

OrthographicCameraController::OrthographicCameraController(
    float aspect_ratio, bool rotation /*= false*/) 
    : aspect_ratio_(aspect_ratio), 
      camera_(-aspect_ratio_ * zoom_level_,
               aspect_ratio_ * zoom_level_,
              -zoom_level_, zoom_level_),
      rotation_(rotation) {}

void OrthographicCameraController::OnEvent(Event& e) {
  EventDispatcher dispatcher(e);
  dispatcher.Dispatch<MouseScrolledEvent>(
      HM_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
  dispatcher.Dispatch<WindowResizeEvent>(
      HM_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
}

void OrthographicCameraController::OnUpdate(Timestep ts) {
  if (Input::IsKeyPressed(HM_KEY_A))
    camera_position_.x -= camera_translation_speed_ * ts;
  if (Input::IsKeyPressed(HM_KEY_D))
    camera_position_.x += camera_translation_speed_ * ts;
  if (Input::IsKeyPressed(HM_KEY_W))
    camera_position_.y += camera_translation_speed_ * ts;
  if (Input::IsKeyPressed(HM_KEY_S))
    camera_position_.y -= camera_translation_speed_ * ts;

  camera_.set_position(camera_position_);
  camera_translation_speed_ = zoom_level_;

  if (rotation_) {
    if (Input::IsKeyPressed(HM_KEY_Q))
      camera_degrees_ += camera_rotation_speed_ * ts;
    if (Input::IsKeyPressed(HM_KEY_E))
      camera_degrees_ -= camera_rotation_speed_ * ts;
    camera_.set_degrees(camera_degrees_);
  }
}

bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e) {
  zoom_level_ -= e.offset_y() * 0.25f;
  zoom_level_ = std::max(zoom_level_, 0.25f);
  camera_.set_projection(-aspect_ratio_ * zoom_level_,
                         aspect_ratio_ * zoom_level_, -zoom_level_,
                         zoom_level_);
  return false;
}

bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e) {
  aspect_ratio_ = (float)e.width() / (float)e.height();
  camera_.set_projection(-aspect_ratio_ * zoom_level_,
                         aspect_ratio_ * zoom_level_, -zoom_level_,
                         zoom_level_);
  return false;
}

}  // namespace hammer