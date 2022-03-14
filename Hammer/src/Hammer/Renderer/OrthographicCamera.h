#pragma once

#include <glm/glm.hpp>

namespace hammer {
class OrthographicCamera {
 public:
  OrthographicCamera(float left, float right, float bottom, float top);

  const glm::vec3& position() const { return position_; }
  void set_position(const glm::vec3 position) {
    position_ = position;
    ReCalculateViewMatrix();
  }
  void set_projection(float left, float right, float bottom, float top);

  inline float degrees() const { return degrees_; }
  void set_degrees(float degrees) {
    degrees_ = degrees;
    ReCalculateViewMatrix();
  }

  const glm::mat4& projection_matrix() const { return projection_matrix_; }
  const glm::mat4& view_matrix() const { return view_matrix_; }
  const glm::mat4& view_projection_matrix() const {
    return view_projection_matrix_;
  }



 private:
  void ReCalculateViewMatrix();

  glm::mat4 view_matrix_;
  glm::mat4 projection_matrix_;
  glm::mat4 view_projection_matrix_;

  glm::vec3 position_ = {0.0f, 0.0f, 0.0f};
  float degrees_ = 0.0f;
  
};
} // namespace hammer
