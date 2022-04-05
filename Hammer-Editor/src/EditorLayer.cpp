#include "EditorLayer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include "Hammer/Scene/SceneSerializer.h"
#include "Hammer/Utils/PlatformUtils.h"
#include "ImGuizmo.h"
#include "Hammer/Math/Math.h"
namespace hammer{
extern const std::filesystem::path global_asset_path;
EditorLayer::EditorLayer()
    : Layer("EditorLayer"),
      camera_controller_(1280.0f / 720.0f),
      square_color_({0.2f, 0.3f, 0.8f, 1.0f}) {}

void EditorLayer::OnAttach() {
  HM_PROFILE_FUNCTION();

  //checkboard_texture_ = Texture2D::Create("assets/textures/Checkerboard.png");
  play_icon_ = Texture2D::Create("resources/icons/PlayButton.png");
  stop_icon_ = Texture2D::Create("resources/icons/StopButton.png");

  // Framebuffer settings
  FramebufferSpecification fb_spec;
  fb_spec.attachments = {FramebufferTextureFormat::RGBA8,
                         FramebufferTextureFormat::RED_INTEGER,
                         FramebufferTextureFormat::Depth};
  fb_spec.width = 1280;
  fb_spec.height = 720;
  framebuffer_ = Framebuffer::Create(fb_spec);

  // Active Scene
  active_scene_ = CreateRef<Scene>();
  //editor_scene_ = active_scene_;
  // EditorCamera is not an entity
  editor_camera_ = EditorCamera(30.0f, 1.778f, 0.1f, 1000.f);
  // App CommandLineArgs to open the default/lastest scene
  auto commandLineArgs = Application::instance().GetCommandLineArgs();
  if (commandLineArgs.count > 1) {
    auto scene_file_path = commandLineArgs[1];
    SceneSerializer serializer(active_scene_);
    serializer.Deserialize(scene_file_path);
  }

#if 0
  // Cameras Entities
  main_camera_entity_ = active_scene_->CreateEntity("Main Camera");
  main_camera_entity_.AddComponent<CameraComponent>();
  second_camera_entity_ =
      active_scene_->CreateEntity("Clip-Space Camera");
  auto& cc = second_camera_entity_.AddComponent<CameraComponent>();
  cc.primary = false;
  // all other Entities here
  square_entity_ = active_scene_->CreateEntity("Green Square");
  square_entity_.AddComponent<SpriteRendererComponent>(
      glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
  auto red_square = active_scene_->CreateEntity("Red Square");
  red_square.AddComponent<SpriteRendererComponent>(
      glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});

  // Init particle here
  // 
  // Script
  class CameraController : public ScriptableEntity {
   public:
     // OnCreate,OnUpdate is using in Scene::OnUpdate,this way will be changed
    virtual void OnCreate() override {
      auto& translation = GetComponent<TransformComponent>().translation;
      translation.x = rand() % 10 - 5.0f;
    }
    virtual void OnDestroy() override {}

    virtual void OnUpdate(Timestep ts) override {
      auto& translation = GetComponent<TransformComponent>().translation;
      float speed = 5.0f;

      if (Input::IsKeyPressed(key::A)) translation.x -= speed * ts;
      if (Input::IsKeyPressed(key::D)) translation.x += speed * ts;
      if (Input::IsKeyPressed(key::W)) translation.y += speed * ts;
      if (Input::IsKeyPressed(key::S)) translation.y -= speed * ts;
    }
  };

  main_camera_entity_.AddComponent<NativeScriptComponent>()
      .Bind<CameraController>();
  second_camera_entity_.AddComponent<NativeScriptComponent>()
      .Bind<CameraController>();
#endif
}
void EditorLayer::OnDetach() { HM_PROFILE_FUNCTION(); }

void EditorLayer::OnUpdate(Timestep ts) {
  HM_PROFILE_FUNCTION();

  // Resize:
  // This solution will render the 'old' sized framebuffer onto the 'new' sized
  // ImGuiPanel and store the 'new' size in m_ViewportSize. The next frame will
  // first resize the framebuffer as m_ViewportSize differs from
  // m_Framebuffer.Width/Height before updating and rendering. This results in
  // never rendering an empty (black) framebuffer.
  // 避免缩放imgui面板时出现的黑屏现象--下一帧立即更新framebuffer
  FramebufferSpecification spec = framebuffer_->GetSpecification();
  // zero sized framebuffer is invalid
  if (viewport_size_.x > 0.0f && viewport_size_.y > 0.0f &&
      (spec.width != viewport_size_.x || spec.height != viewport_size_.y)) {
    framebuffer_->Resize((uint32_t)viewport_size_.x,
                         (uint32_t)viewport_size_.y);
    camera_controller_.OnResize(viewport_size_.x, viewport_size_.y);
    editor_camera_.SetViewportSize(viewport_size_.x, viewport_size_.y);
    active_scene_->OnViewportResize((uint32_t)viewport_size_.x,
                                    (uint32_t)viewport_size_.y);
  }

  // Render
  Renderer2D::ResetStats();
  {
    HM_PROFILE_SCOPE("Renderer Preperation");
    framebuffer_->Bind();
    RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    RenderCommand::Clear();
  }
  // Clear our entity ID attachment to -1
  framebuffer_->ClearAttachment(1, -1);

  // Update scene
  switch (scene_state_) {
    case hammer::EditorLayer::SceneState::EDIT: {
      // if (viewport_focused_) camera_controller_.OnUpdate(ts);
      editor_camera_.OnUpdate(ts);
      active_scene_->OnUpdateEditor(ts,editor_camera_);
      break;
    }
    case hammer::EditorLayer::SceneState::PLAY: {
      active_scene_->OnUpdateRuntime(ts);
    }
      break;
    default:
      break;
  }

  auto [mx, my] = ImGui::GetMousePos();// GetMousePos use global coordinate
  // viewport_bounds[0] is {min_bound.x, min_bound.y},the left-top of viewport bound
  // viewport_bounds[1] is {max_bound.x, max_bound.y},the right-bottom of viewport bound
  // calculate the mouse pos in view port
  // 计算鼠标光标相对于view port 的相对坐标,imgui 从上往西渲染 
  mx = mx - viewport_bounds[0].x;
  my = my - viewport_bounds[0].y;
  glm::vec2 viewport_size = viewport_bounds[1] - viewport_bounds[0];
  // my's coordinate is related to the bottom
  // my 相对于底部的坐标
  my = viewport_size.y - my;
  int mouse_x = (int)mx;
  int mouse_y = (int)my;

  if (mouse_x >= 0 && mouse_y >=0 && mouse_x <(int)viewport_size.x && 
      mouse_y < (int)viewport_size.y) {
    int pixel_data = framebuffer_->ReadPixel(1, mouse_x, mouse_y);
    hovered_entity_ = pixel_data == -1 ? 
      Entity() : Entity((entt::entity)pixel_data, active_scene_.get());
  }
  OnOverlayRender();
  framebuffer_->Unbind();
}

void EditorLayer::OnImGuiRender() {
  HM_PROFILE_FUNCTION();

  // Note: Switch this to true to enable dock_space
  static bool dockingEnabled = true;

  static bool dockspaceOpen = true;
  static bool opt_fullscreen_persistant = true;
  bool opt_fullscreen = opt_fullscreen_persistant;
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

  // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
  // window not dockable into, because it would be confusing to have two
  // docking targets within each others.
  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  if (opt_fullscreen) {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |=
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  }

  // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will
  // render our background and handle the pass-thru hole, so we ask Begin() to
  // not render a background.
  if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    window_flags |= ImGuiWindowFlags_NoBackground;

  // Important: note that we proceed even if Begin() returns false (aka window
  // is collapsed). This is because we want to keep our DockSpace() active. If
  // a DockSpace() is inactive, all active windows docked into it will lose
  // their parent and become undocked. We cannot preserve the docking
  // relationship between an active window and an inactive docking, otherwise
  // any change of dockspace/settings would lead to windows being stuck in
  // limbo and never being visible.
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
  ImGui::PopStyleVar();

  if (opt_fullscreen) ImGui::PopStyleVar(2);

  // DockSpace
  ImGuiIO& io = ImGui::GetIO();
  ImGuiStyle& style = ImGui::GetStyle();
  float minWinSizeX = style.WindowMinSize.x;
  style.WindowMinSize.x = 370.0f;
  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
  }
  style.WindowMinSize.x = minWinSizeX;

  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      // Disabling full screen would allow the window to be moved to the front
      // of other windows, which we can't undo at the moment without finer
      // window depth/z control.
      // ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
      
      if (ImGui::MenuItem("New", "Ctrl+N")) NewScene();

      if (ImGui::MenuItem("Open...", "Ctrl+O")) OpenScene();

      if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) SaveSceneAs();

      if (ImGui::MenuItem("Exit")) Application::instance().Close();
      ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
  }
  // Set SceneHierarchyPanel
  scene_hierarchy_panel_.OnImGuiRender();
  // Set ContentBrowserPanel
  content_browser_panel_.OnImGuiRender();
  // Set Component Stats
  ImGui::Begin("Stats");
  std::string name = "None";
  if (hovered_entity_) 
    name = hovered_entity_.GetComponent<TagComponent>().tag;
  ImGui::Text("Hovered Entity: %s", name.c_str());
  auto stats = Renderer2D::GetStats();
  ImGui::Text("Renderer2D Stats:");
  ImGui::Text("Draw Calls: %d", stats.draw_calls);
  ImGui::Text("Quads: %d", stats.quad_count);
  ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
  ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
  ImGui::End();  // "Stats"

  ImGui::Begin("Settings");
  ImGui::Checkbox("Show physics colliders", &show_physics_colliders_);
  ImGui::End();

  // ImGui Viewport
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("Viewport");
  auto viewport_min_region = ImGui::GetWindowContentRegionMin();
  auto viewport_max_region = ImGui::GetWindowContentRegionMax();
  auto viewport_offset = ImGui::GetWindowPos();
  viewport_bounds[0] = {viewport_min_region.x + viewport_offset.x,
                        viewport_min_region.y + viewport_offset.y};
  viewport_bounds[1] = {viewport_max_region.x + viewport_offset.x,
                        viewport_max_region.y + viewport_offset.y};
  viewport_focused_ = ImGui::IsWindowFocused();
  viewport_hovered_ = ImGui::IsWindowHovered();
  Application::instance().GetImGuiLayer()->BlockEvents(!viewport_focused_ &&
                                                       !viewport_hovered_);

  ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
  viewport_size_ = {viewport_panel_size.x, viewport_panel_size.y};

  // framebuffer
  uint64_t textureID = framebuffer_->GetColorAttachmentRendererID();
  ImGui::Image(reinterpret_cast<void*>(textureID),
               ImVec2{viewport_size_.x, viewport_size_.y}, ImVec2{0, 1},
               ImVec2{1, 0});

  // DragDrop operation
  if (ImGui::BeginDragDropTarget()) {
    if (const ImGuiPayload* payload =
            ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
      const wchar_t* path = (const wchar_t*)payload->Data;
      OpenScene(std::filesystem::path(global_asset_path) / path);

    }
    ImGui::EndDragDropTarget();
  }
  // Gizmos
  Entity selected_entity = scene_hierarchy_panel_.GetSelectedEntity();
  if (selected_entity && gizmo_type_ != -1) {
    ImGuizmo::SetOrthographic(false); 
    ImGuizmo::SetDrawlist();

    // imguizmo viewport operation/manipulate
    ImGuizmo::SetRect(viewport_bounds[0].x, viewport_bounds[0].y,
                      viewport_bounds[1].x - viewport_bounds[0].x,
                      viewport_bounds[1].y - viewport_bounds[0].y);
    
    // Cameras:
    // Runtime camera from entity 
    /*auto camera_entity = active_scene_->GetPrimaryCameraEntity();
    const auto& camera = camera_entity.GetComponent<CameraComponent>().camera;
    const glm::mat4& camera_projection = camera.projection();
    glm::mat4 camera_view_matrix = glm::inverse(
        camera_entity.GetComponent<TransformComponent>().GetTransform());*/

    // Editor Camera
    const glm::mat4& camera_projection = editor_camera_.projection();
    glm::mat4 camera_view_matrix = editor_camera_.GetViewMatrix();
    // Entity transform
    auto& tc = selected_entity.GetComponent<TransformComponent>();
    glm::mat4 transform = tc.GetTransform();

    // Snapping transform
    bool snap = Input::IsKeyPressed(key::LeftControl);
    float snap_value = 0.5f;// Snap to 0.5m for translation/scale
    // Snap to 45 degrees for rotation
    if (gizmo_type_ == ImGuizmo::OPERATION::ROTATE) snap_value = 45.0f;
    float snap_values[3] = {snap_value, snap_value, snap_value};

    ImGuizmo::Manipulate(glm::value_ptr(camera_view_matrix),
                         glm ::value_ptr(camera_projection),
                         (ImGuizmo::OPERATION)gizmo_type_, ImGuizmo::LOCAL,
                         glm::value_ptr(transform),nullptr,snap ? snap_values : nullptr);

    if (ImGuizmo::IsUsing()) {
      glm::vec3 translation, rotation, scale;
      math::DecomposeTranform(transform, translation, rotation, scale);
      glm::vec3 delta_rotation = rotation - tc.rotation;
      tc.translation = translation;
      tc.rotation += delta_rotation;
      tc.scale = scale;
    }
  }

  ImGui::End();  // "Viewport"
  ImGui::PopStyleVar();
  
  UI_Toolbar();

  ImGui::End();  // "DockSpace Demo"
}

void EditorLayer::OnEvent(Event& e) {
  camera_controller_.OnEvent(e);
  editor_camera_.OnEvent(e);
  EventDispatcher dispatcher(e);
  dispatcher.Dispatch<KeyPressedEvent>(
      HM_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
  dispatcher.Dispatch<MouseButtonPressedEvent>(
      HM_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
}


bool EditorLayer::OnKeyPressed(KeyPressedEvent& e) {
  if (e.repeat_count() > 0) return false;
  bool control = Input::IsKeyPressed(key::LeftControl) ||
                 Input::IsKeyPressed(key::RightControl);
  bool shift = Input::IsKeyPressed(key::LeftShift) ||
               Input::IsKeyPressed(key::RightShift);
  switch (e.keycode()) { 
    case key::N: {
      if (control) NewScene();
      break;
    }
    case key::O: {
      if (control) OpenScene();
      break;
    }
    case key::S: {
      if (control) {
        if (shift)
          SaveSceneAs();
        else 
          SaveScene();
      }
      break;
    }
    // Scene Commands
    case key::D: {
      if (control) OnDuplicateEntity();
      break;
    }
    // Only change type when user is currently not using an ImGuizmo
    case key::Q: {
      if (!ImGuizmo::IsUsing())
        gizmo_type_ = -1;
      break;
    }
    case key::W: {
      if (!ImGuizmo::IsUsing()) 
        gizmo_type_ = ImGuizmo::OPERATION::TRANSLATE;
      break;
    }
    case key::E: {
      if (!ImGuizmo::IsUsing()) 
        gizmo_type_ = ImGuizmo::OPERATION::ROTATE;
      break;
    }
    case key::R: {
      if (!ImGuizmo::IsUsing()) 
        gizmo_type_ = ImGuizmo::OPERATION::SCALE;
      break;
    }
  }
  return false;
}

bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
  if (e.button() == mouse::ButtonLeft) {
    if (viewport_hovered_ && !ImGuizmo::IsOver() &&
        !Input::IsKeyPressed(key::LeftAlt))
      scene_hierarchy_panel_.SetSelectedEntity(hovered_entity_);
  }
  return false;
}

void EditorLayer::NewScene() { 
  //if (scene_state_ == SceneState::PLAY) return;
  active_scene_ = CreateRef<Scene>();
  active_scene_->OnViewportResize((uint32_t)viewport_size_.x,
                                  (uint32_t)viewport_size_.y);
  scene_hierarchy_panel_.SetContext(active_scene_);
  editor_scene_path_ = std::filesystem::path();
}

void EditorLayer::OpenScene() {
  std::string file_path =
      FileDialogs::OpenFile("Hammer Scene (*.hammer)\0*.hammer\0");
  if (!file_path.empty()) {
    OpenScene(file_path);
  }
}

void EditorLayer::OpenScene(const std::filesystem::path& path) {
  if (scene_state_ != SceneState::EDIT) OnSceneStop();

  if (path.extension().string() != ".hammer") {
    HM_WARN("Could not load {0} - it is not a scene file", path.filename().string());
    return;
  }
  Ref<Scene> new_scene = CreateRef<Scene>();
  SceneSerializer serializer(new_scene);
  if (serializer.Deserialize(path.string())) {
    editor_scene_ = new_scene;
    editor_scene_->OnViewportResize((uint32_t)viewport_size_.x,
                                    (uint32_t)viewport_size_.y);
    scene_hierarchy_panel_.SetContext(editor_scene_);
    active_scene_ = editor_scene_;
    editor_scene_path_ = path;
  }
}

void EditorLayer::SaveScene() {
  if (!editor_scene_path_.empty())
    SerializeScene(active_scene_, editor_scene_path_);
  else
    SaveSceneAs();
}

void EditorLayer::SaveSceneAs() {
  std::string file_path =
      FileDialogs::SaveFile("Hammer Scene (*.hammer)\0*.hammer\0");
  if (!file_path.empty()) {
    SerializeScene(active_scene_, file_path);
    editor_scene_path_ = file_path;
  }
}

void EditorLayer::SerializeScene(Ref<Scene> scene,
                                 const std::filesystem::path& path) {
  SceneSerializer serializer(scene);
  serializer.Serialize(path.string());
}

void EditorLayer::OnScenePlay() {
  scene_state_ = SceneState::PLAY;
  if (editor_scene_) {
    active_scene_ = Scene::Copy(editor_scene_);
  }
  active_scene_->OnRuntimeStart();
  scene_hierarchy_panel_.SetContext(active_scene_);
}

void EditorLayer::OnSceneStop() {
  scene_state_ = SceneState::EDIT;
  active_scene_->OnRuntimeStop();
  if (editor_scene_) {
    active_scene_ = editor_scene_;
  }
  scene_hierarchy_panel_.SetContext(active_scene_);
}

void EditorLayer::OnDuplicateEntity() {
  if (scene_state_ != SceneState::EDIT) return;
  Entity selected_entity = scene_hierarchy_panel_.GetSelectedEntity();
  if (selected_entity) editor_scene_->DuplicateEntity(selected_entity);
}

void EditorLayer::UI_Toolbar() {
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
  auto& colors = ImGui::GetStyle().Colors;
  const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
  ImGui::PushStyleColor(
      ImGuiCol_ButtonHovered,
      ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
  const auto& buttonActive = colors[ImGuiCol_ButtonActive];
  ImGui::PushStyleColor(
      ImGuiCol_ButtonActive,
      ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));
  ImGui::Begin("##toolbar", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_NoScrollWithMouse |
                   ImGuiWindowFlags_NoTitleBar);
  float size = ImGui::GetWindowHeight() - 4.0f;
  Ref<Texture2D> icon =
      scene_state_ == SceneState::EDIT ? play_icon_ : stop_icon_;
  ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) -
                       (size * 0.5f));
  if (ImGui::ImageButton((ImTextureID)icon->GetRenderID(), ImVec2(size, size),
                         ImVec2(0, 0), ImVec2(1, 1), 0)) {
    PlayOrEdit();
  }
  ImGui::PopStyleVar(2);
  ImGui::PopStyleColor(3);
  ImGui::End();
}

void EditorLayer::PlayOrEdit() {
  if (scene_state_ == SceneState::EDIT)
    OnScenePlay();
  else if (scene_state_ == SceneState::PLAY)
    OnSceneStop();
}

void EditorLayer::OnOverlayRender() {
   Entity camare_entity = active_scene_->GetPrimaryCameraEntity();
  if (scene_state_ == SceneState::PLAY && camare_entity) {
    Renderer2D::BeginScene(
        camare_entity.GetComponent<CameraComponent>().camera,
        camare_entity.GetComponent<TransformComponent>().GetTransform());
  } else {
    Renderer2D::BeginScene(editor_camera_);
  }
  if (show_physics_colliders_) {
    // Show Box Colliders
    {
      auto view = active_scene_->GetAllEntitiesWith<TransformComponent,
                                                    BoxCollider2DComponent>();
        for (auto entity : view) {
          auto [tc, bc2d] =
              view.get<TransformComponent, BoxCollider2DComponent>(entity);

          glm::vec3 translation =
              tc.translation + glm::vec3(bc2d.offset, 0.001f);
          glm::vec3 scale = tc.scale * glm::vec3(bc2d.size * 2.0f, 1.0f);
          glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation) *
                                glm::rotate(glm::mat4(1.0f), tc.rotation.z,
                                            glm::vec3(0.0f, 0.0f, 1.0f)) *
                                glm::scale(glm::mat4(1.0f), scale);

          Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
        }
    }
    // Show Circle Colliders
    {
      auto view = active_scene_->GetAllEntitiesWith<TransformComponent,
                                                    CircleCollider2DComponent>();
        for (auto entity : view) {
          auto [tc, cc2d] =
              view.get<TransformComponent, CircleCollider2DComponent>(entity);

          glm::vec3 translation =
              tc.translation + glm::vec3(cc2d.offset, 0.001f);
          glm::vec3 scale = tc.scale * glm::vec3(cc2d.radius * 2.0f);
          glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation) *
                                glm::scale(glm::mat4(1.0f), scale);

          Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.02f);
        }
    }
  }
  Renderer2D::EndScene();
}

}  // namespace hammer
