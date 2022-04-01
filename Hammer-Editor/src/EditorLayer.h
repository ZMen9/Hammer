#pragma once
#include "Hammer.h"
#include "Panels/SceneHierarchyPanel.h"
namespace hammer {
class EditorLayer : public Layer {
 public:
  EditorLayer();
  virtual void OnAttach() override;
  virtual void OnDetach() override;

  virtual void OnUpdate(Timestep ts) override;
  virtual void OnImGuiRender() override;
  void OnEvent(Event& e) override;

 private:
  OrthographicCameraController camera_controller_;
  bool viewport_focused = false, viewport_hovered = false;

  Ref<VertexArray> square_va_;
  Ref<Shader> flat_color_shader_;
  Ref<Framebuffer> framebuffer_;

  Ref<Scene> active_scene_;
  Entity main_camera_entity_;
  Entity second_camera_entity_;
  bool primary_camera_ = true;
  Entity square_entity_;

  Ref<Texture2D> checkboard_texture_;

  glm::vec4 square_color_ = {0.2f, 0.3f, 0.8f, 1.0f};
  glm::vec2 viewport_size_ = {0.0f, 0.0f};

  // Panels
  SceneHierarchyPanel scene_hierarchy_panel_;
};
}  // namespace hammer