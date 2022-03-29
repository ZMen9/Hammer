#include "hmpch.h"
#include "Hammer/Renderer/OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>


namespace hammer {
OrthographicCamera::OrthographicCamera(float left, float right,
                                       float bottom, float top)
    : projection_matrix_(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)),
      view_matrix_(1.0f) {
  HM_PROFILE_FUNCTION();
  view_projection_matrix_ = projection_matrix_ * view_matrix_;
}

void OrthographicCamera::set_projection(float left, float right, float bottom,
                                        float top) {
  HM_PROFILE_FUNCTION();
  projection_matrix_ = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
  view_projection_matrix_ = projection_matrix_ * view_matrix_;
}

void OrthographicCamera::ReCalculateViewMatrix() {
  HM_PROFILE_FUNCTION();
  glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), position_) *
      glm::rotate(glm::mat4(1.0f), glm::radians(degrees_), glm::vec3(0, 0, 1));
  /*glm::mat4 transform = glm::translate(glm::mat4(1.0f), position_);
  transform =
      glm::rotate(transform, glm::radians(degrees_), glm::vec3(0, 0, 1));*/
  view_matrix_ = glm::inverse(transform);//glm::lookAt
  view_projection_matrix_ = projection_matrix_ * view_matrix_;
}

}  // namespace hammer
