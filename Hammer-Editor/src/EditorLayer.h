#pragma once
#include "Hammer.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Hammer/Renderer/EditorCamera.h"
#include "Panels/ContentBrowserPanel.h"
namespace hammer {
class EditorLayer : public Layer {
 public:
  EditorLayer();
  virtual void OnAttach() override;
  virtual void OnDetach() override;

  virtual void OnUpdate(Timestep ts) override;
  virtual void OnImGuiRender() override;
  virtual void OnEvent(Event& e) override;

 private:
  bool OnKeyPressed(KeyPressedEvent& e);
  bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

  void NewScene();
  void OpenScene();  
  void OpenScene(const std::filesystem::path& path);
  void SaveScene();
  void SaveSceneAs();

  void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

  void OnScenePlay();
  void OnSceneStop();
  void OnDuplicateEntity();

  // Play or Edit UI Panels
  void UI_Toolbar();
  void PlayOrEdit();
  void OnOverlayRender();
 private:
  OrthographicCameraController camera_controller_;
  Ref<Scene> active_scene_;
  // Panels
  SceneHierarchyPanel scene_hierarchy_panel_;
  ContentBrowserPanel content_browser_panel_;
  Ref<Framebuffer> framebuffer_;
  EditorCamera editor_camera_;
  int gizmo_type_ = -1;
  bool viewport_focused_ = false, viewport_hovered_ = false;
  glm::vec2 viewport_bounds[2];
  glm::vec2 viewport_size_ = {0.0f, 0.0f};
  bool primary_camera_ = true;
  Entity hovered_entity_;
  enum class SceneState {
    EDIT = 0, PLAY = 1
  };
  SceneState scene_state_ = SceneState::EDIT;
  Ref<Scene> editor_scene_;
  std::filesystem::path editor_scene_path_;
  // Editor Resources
  Ref<Texture2D> play_icon_;
  Ref<Texture2D> stop_icon_;
  // show_physics_colliders_
  bool show_physics_colliders_ = false;
  //----temp----------------------
  Ref<VertexArray> square_va_;
  Ref<Shader> flat_color_shader_;
  Entity main_camera_entity_;
  Entity second_camera_entity_;
  Entity square_entity_;
  Ref<Texture2D> checkboard_texture_;
  glm::vec4 square_color_ = {0.2f, 0.3f, 0.8f, 1.0f};
  //----temp----------------------
};
}  // namespace hammer