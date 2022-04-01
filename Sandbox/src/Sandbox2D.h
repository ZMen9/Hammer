#pragma once

#include "Hammer.h"
#include "ParticleSystem.h"

class Sandbox2D : public hammer::Layer {
 public:
  Sandbox2D();
  virtual void OnAttach() override;
  virtual void OnDetach() override;

  virtual void OnUpdate(hammer::Timestep ts) override;
  virtual void OnImGuiRender() override;
  void OnEvent(hammer::Event& e) override;
 private:
  hammer::OrthographicCameraController camera_controller_;

  hammer::Ref<hammer::VertexArray> square_va_;
  hammer::Ref<hammer::Shader> flat_color_shader_;
  hammer::Ref<hammer::Framebuffer> framebuffer_;
  
  hammer::Ref<hammer::Texture2D> checkboard_texture_;

  glm::vec4 square_color_ = {0.2f, 0.3f, 0.8f, 1.0f};

  ParticleProps particle_;
  ParticleSystem particle_system_;

};