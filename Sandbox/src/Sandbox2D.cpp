#include "Sandbox2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>


Sandbox2D::Sandbox2D()
    : Layer("Sandbox2D"), camera_controller_(1920.0f / 1080.0f),
      square_color_({0.2f, 0.3f, 0.8f, 1.0f}) {}

void Sandbox2D::OnAttach() {
  HM_PROFILE_FUNCTION();

  checkboard_texture_ = hammer::Texture2D::Create("assets/textures/Checkerboard.png");
  sprite_sheet_ =
      hammer::Texture2D::Create("assets/game/texture/RPGpack_sheet_2X.png");
  sub_texture_stair_ = hammer::SubTexture2D::CreateUseCoords(sprite_sheet_, {7 , 6},
                                                       {128, 128});
  sub_texture_barrel_ =
      hammer::SubTexture2D::CreateUseCoords(sprite_sheet_, {8, 2}, {128, 128});
  sub_texture_tree_ = hammer::SubTexture2D::CreateUseCoords(
      sprite_sheet_, {2, 1}, {128, 128}, {1, 2});


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
    hammer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    hammer::RenderCommand::Clear();
  }
#if 0
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
#endif
  // texture atlas
  hammer::Renderer2D::BeginScene(camera_controller_.GetCamera());
  hammer::Renderer2D::DrawQuad({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, sub_texture_stair_);
  hammer::Renderer2D::DrawQuad({1.0f, 0.0f, 0.0f}, {1.0f, 1.0f},
                               sub_texture_barrel_);
  hammer::Renderer2D::DrawQuad({-2.0f, 0.0f, 0.0f}, {1.0f, 2.0f},
                               sub_texture_tree_);
  hammer::Renderer2D::EndScene();

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

}

void Sandbox2D::OnImGuiRender() {
  HM_PROFILE_FUNCTION();
  ImGui::Begin("Settings");

  auto stats = hammer::Renderer2D::GetStats();
  ImGui::Text("Renderer2D Stats:");
  ImGui::Text("Draw Calls: %d", stats.draw_calls);
  ImGui::Text("Quads: %d", stats.quad_count);
  ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
  ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
  ImGui::ColorEdit4("Square Color", glm::value_ptr(square_color_));

  ImGui::End();
}

void Sandbox2D::OnEvent(hammer::Event& e) { camera_controller_.OnEvent(e); }
