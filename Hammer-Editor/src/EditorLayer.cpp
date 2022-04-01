#include "EditorLayer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include "Hammer/Scene/SceneSerializer.h"

namespace hammer{
EditorLayer::EditorLayer()
    : Layer("EditorLayer"),
      camera_controller_(1280.0f / 720.0f),
      square_color_({0.2f, 0.3f, 0.8f, 1.0f}) {}

void EditorLayer::OnAttach() {
  HM_PROFILE_FUNCTION();

  checkboard_texture_ = Texture2D::Create("assets/textures/Checkerboard.png");

  // Framebuffer settings
  FramebufferSpecification fb_spec;
  fb_spec.width = 1280;
  fb_spec.height = 720;
  framebuffer_ = Framebuffer::Create(fb_spec);

  // Active Scene
  active_scene_ = CreateRef<Scene>();
  // SceneHierarchyPanel
  scene_hierarchy_panel_.SetContext(active_scene_);
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
    active_scene_->OnViewportResize((uint32_t)viewport_size_.x,
                                    (uint32_t)viewport_size_.y);
  }

  // Update
  if (viewport_focused)
    camera_controller_.OnUpdate(ts);

  // Render
  Renderer2D::ResetStats();
  {
    HM_PROFILE_SCOPE("Renderer Preperation");
    framebuffer_->Bind();
    RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    RenderCommand::Clear();
  }
 // Update scene
  active_scene_->OnUpdate(ts);
  framebuffer_->Unbind();
}

void EditorLayer::OnImGuiRender() {
  HM_PROFILE_FUNCTION();

  // Note: Switch this to true to enable dockspace
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

      if (ImGui::MenuItem("Serialize")) {
        SceneSerializer serializer(active_scene_);
        serializer.Serialize("assets/scenes/Example.hammer");
      }

      if (ImGui::MenuItem("Deserialize")) {
        SceneSerializer serializer(active_scene_);
        serializer.Deserialize("assets/scenes/Example.hammer");
      }

      if (ImGui::MenuItem("Exit")) Application::instance().Close();
      ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
  }

  // SceneHierarchyPanel
  scene_hierarchy_panel_.OnImGuiRender();

  // Component Stats
  ImGui::Begin("Stats");
  auto stats = Renderer2D::GetStats();
  ImGui::Text("Renderer2D Stats:");
  ImGui::Text("Draw Calls: %d", stats.draw_calls);
  ImGui::Text("Quads: %d", stats.quad_count);
  ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
  ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

  if (ImGui::Checkbox("Main Camera", &primary_camera_)) {
    main_camera_entity_.GetComponent<CameraComponent>().primary =
        primary_camera_;
    second_camera_entity_.GetComponent<CameraComponent>().primary =
        !primary_camera_;
  }


  ImGui::End();  // "Stats"

  // ImGui Viewport
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("Viewport");
  viewport_focused = ImGui::IsWindowFocused();
  viewport_hovered = ImGui::IsWindowHovered();
  Application::instance().GetImGuiLayer()->BlockEvents(!viewport_focused ||
                                                       !viewport_hovered);

  ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
  viewport_size_ = {viewport_panel_size.x, viewport_panel_size.y};

  // framebuffer
  uint64_t textureID = framebuffer_->GetColorAttachmentRendererID();
  ImGui::Image(reinterpret_cast<void*>(textureID),
               ImVec2{viewport_size_.x, viewport_size_.y}, ImVec2{0, 1},
               ImVec2{1, 0});
  ImGui::PopStyleVar();
  ImGui::End();  // "Viewport"

  ImGui::End();  // "DockSpace Demo"
}

void EditorLayer::OnEvent(Event& e) { camera_controller_.OnEvent(e); }
}  // namespace hammer
