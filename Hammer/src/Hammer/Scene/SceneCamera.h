#pragma once

#include "Hammer/Renderer/Camera.h"

namespace hammer {
class SceneCamera : public Camera {
 public:
  enum class ProjectionType { Perspective = 0, Orthographic = 1
  };
 public:
  SceneCamera();
  virtual ~SceneCamera() = default;
  // SetViewportSize is about aspect ratio
  void SetViewportSize(uint32_t width, uint32_t height);
  void SetOrthographic(float size, float near_clip, float far_clip);
  void SetOrthographicSize(float size) {
    orthographic_size_ = size;
    RecalculateProjection();
  }
  float GetOrthographicSize() const { return orthographic_size_; }

  void SetOrthographicNearClip(float near_clip) {
    orthographic_near_ = near_clip;
    RecalculateProjection();
  }
  float GetOrthographicNearClip() const { return orthographic_near_; }

  void SetOrthographicFarClip(float far_clip) {
    orthographic_far_ = far_clip;
    RecalculateProjection();
  }
  float GetOrthographicFarClip() const { return orthographic_far_; }

  void SetPerspective(float vertical_fov, float near_clip, float far_clip);
  void SetPerspectiveVerticalFOV(float vertical_fov) {
    perspective_fov_ = vertical_fov;
    RecalculateProjection();
  }
  float GetPerspectiveVerticalFOV() const { return perspective_fov_; }

  void SetPerspectiveNearClip(float near_clip) {
    perspective_near_ = near_clip;
    RecalculateProjection();
  }
  float GetPerspectiveNearClip() const { return perspective_near_; }

  void SetPerspectiveFarClip(float far_clip) {
    perspective_far_ = far_clip;
    RecalculateProjection();
  }
  float GetPerspectiveFarClip() const { return perspective_far_; }

  ProjectionType GetProjectionType() const { return projection_type_; }
  void SetProjectionType(ProjectionType type) {
    projection_type_ = type;
    RecalculateProjection();
  }
 private:
  void RecalculateProjection();

 private:
  ProjectionType projection_type_ = ProjectionType::Orthographic;
  float perspective_fov_ = glm::radians(45.0f);
  float perspective_near_ = 0.01f, perspective_far_ = 1000.0f;
  float orthographic_size_= 10.0f;
  float orthographic_near_ = -1.0f, orthographic_far_ = 1.0f;

  float aspect_ratio_ = 0.0f;
};
}