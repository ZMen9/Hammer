#include "Sandbox2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>


Sandbox2D::Sandbox2D()
    : Layer("Sandbox2D"), camera_controller_(1280.0f / 720.0f),
      square_color_({0.2f, 0.3f, 0.8f, 1.0f}) {}

void Sandbox2D::OnAttach() {
  HM_PROFILE_FUNCTION();
  camera_controller_.set_zoom_level(5.0f); 
  checkboard_texture_ = hammer::Texture2D::Create("assets/textures/Checkerboard.png");

  // Framebuffer settings
  hammer::FramebufferSpecification fb_spec;
  fb_spec.width = 1280;
  fb_spec.height = 720;
  framebuffer_ = hammer::Framebuffer::Create(fb_spec);

  // Init particle
  particle_.ColorBegin = {254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f};
  particle_.ColorEnd = {254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f};
  particle_.SizeBegin = 0.5f, particle_.SizeVariation = 0.3f,
  particle_.SizeEnd = 0.0f;
  particle_.LifeTime = 1.0f;
  particle_.Velocity = {0.0f, 0.0f};
  particle_.VelocityVariation = {3.0f, 1.0f};
  particle_.Position = {0.0f, 0.0f};
}

void Sandbox2D::OnDetach() { HM_PROFILE_FUNCTION(); }

void Sandbox2D::OnUpdate(hammer::Timestep ts) {
  HM_PROFILE_FUNCTION();

  // Update

  camera_controller_.OnUpdate(ts);

  // Render
  hammer::Renderer2D::ResetStats();
  {
    HM_PROFILE_SCOPE("Renderer Preperation");
    framebuffer_->Bind();
    hammer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    hammer::RenderCommand::Clear();
  }

  {
    static float degree = 0.0f;
    degree += ts * 50.0f;

    HM_PROFILE_SCOPE("Renderer Draw");
    hammer::Renderer2D::BeginScene(camera_controller_.GetCamera());
    hammer::Renderer2D::DrawRotatedQuad({1.0f, 0.0f}, {0.8f, 0.8f}, -45.0f,
                                        {0.8f, 0.2f, 0.3f, 1.0f});
    hammer::Renderer2D::DrawQuad({-1.0f, 0.0f}, {0.8f, 0.8f},
                                 {0.8f, 0.2f, 0.3f, 1.0f});
    hammer::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, square_color_);
    hammer::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, {20.0f, 20.0f},
                                 checkboard_texture_, 10.0f);
    hammer::Renderer2D::DrawRotatedQuad({-2.0f, 0.0f, 0.0f}, {1.0f, 1.0f},
                                        degree, checkboard_texture_, 20.0f);
    hammer::Renderer2D::EndScene();

    hammer::Renderer2D::BeginScene(camera_controller_.GetCamera());
    for (float y = -5.0f; y < 5.0f; y += 0.5f) {
      for (float x = -5.0f; x < 5.0f; x += 0.5f) {
        glm::vec4 color = {(x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f};
        hammer::Renderer2D::DrawQuad({x, y}, {0.45f, 0.45f}, color);
      }
    }
    hammer::Renderer2D::EndScene();
  }
#if 0
// particle test
  if (hammer::Input::IsMouseButtonPressed(HM_MOUSE_BUTTON_RIGHT)) {
    auto [x, y] = hammer::Input::GetMousePosition();
    auto width = hammer::Application::instance().window().width();
    auto height = hammer::Application::instance().window().height();

    auto bounds = camera_controller_.GetBounds();
    auto pos = camera_controller_.GetCamera().position();
    x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
    y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
    particle_.Position = {x + pos.x, y + pos.y};
    for (int i = 0; i < 5; i++) particle_system_.Emit(particle_);
  }

  particle_system_.OnUpdate(ts);
  particle_system_.OnRender(camera_controller_.GetCamera());
#endif
  
  framebuffer_->Unbind();
}

void Sandbox2D::OnImGuiRender() {
  HM_PROFILE_FUNCTION();

  // Note: Switch this to true to enable dockspace
  static bool dockingEnabled = true;
  if (dockingEnabled) {
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
      window_flags |= ImGuiWindowFlags_NoTitleBar |
                      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                      ImGuiWindowFlags_NoMove;
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
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
      ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
      ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        // Disabling fullscreen would allow the window to be moved to the front
        // of other windows, which we can't undo at the moment without finer
        // window depth/z control.
        // ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

        if (ImGui::MenuItem("Exit")) hammer::Application::instance().Close();
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }
    ImGui::Begin("Settings");

    auto stats = hammer::Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", stats.draw_calls);
    ImGui::Text("Quads: %d", stats.quad_count);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
    ImGui::ColorEdit4("Square Color", glm::value_ptr(square_color_));
    uint32_t texture_id = framebuffer_->GetColorAttachmentRendererID();
    ImGui::Image((void*)texture_id, ImVec2{1280, 720});

    ImGui::End();//Settins

    ImGui::End();//DockSpace Demo
  } else {
    ImGui::Begin("Settings");

    auto stats = hammer::Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", stats.draw_calls);
    ImGui::Text("Quads: %d", stats.quad_count);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
    ImGui::ColorEdit4("Square Color", glm::value_ptr(square_color_));
    uint32_t id = checkboard_texture_->GetRenderID();
    void* texture_id = &id;
    ImGui::Image((void*)texture_id, ImVec2{1280.0f, 720.0f});
    ImGui::End();
  }
}

void Sandbox2D::OnEvent(hammer::Event& e) { camera_controller_.OnEvent(e); }
