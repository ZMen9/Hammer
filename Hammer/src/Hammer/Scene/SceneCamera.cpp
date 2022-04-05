#include "hmpch.h"
#include "Hammer/Scene/SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>


namespace hammer {

SceneCamera::SceneCamera() { RecalculateProjection(); }

void SceneCamera::SetOrthographic(float size, float near_clip, float far_clip) {
  projection_type_ = ProjectionType::Orthographic;
  orthographic_size_ = size;
  orthographic_near_ = near_clip;
  orthographic_far_ = far_clip;
  RecalculateProjection();
}

void SceneCamera::SetPerspective(float vertical_fov, float near_clip,
                                 float far_clip) {
  projection_type_ = ProjectionType::Perspective;
  perspective_fov_ = vertical_fov;
  perspective_near_ = near_clip;
  perspective_far_ = far_clip;
  RecalculateProjection();
}

void SceneCamera::SetViewportSize(uint32_t width, uint32_t height) {
  HM_CORE_ASSERT(width > 0 && height > 0);
  aspect_ratio_ = (float)width / (float)height;
  RecalculateProjection();
}

void SceneCamera::RecalculateProjection() {
  if (projection_type_ == ProjectionType::Perspective) {
    projection_ = glm::perspective(perspective_fov_, aspect_ratio_,
                                   perspective_near_, perspective_far_);

  } else if (projection_type_ == ProjectionType::Orthographic) {
    float ortho_left = -orthographic_size_ * aspect_ratio_ * 0.5f;
    float ortho_right = orthographic_size_ * aspect_ratio_ * 0.5f;
    float ortho_bottom = -orthographic_size_ * 0.5f;
    float ortho_top = orthographic_size_ * 0.5f;
    projection_ = glm::ortho(ortho_left, ortho_right, ortho_bottom, ortho_top,
                             orthographic_near_, orthographic_far_);
  }

}
 
}  // namespace hammer