#pragma once

#include "Hammer.h"

class ExampleLayer : public hammer::Layer {
 public:
  ExampleLayer();
  virtual ~ExampleLayer() = default;

  virtual void OnAttach() override;
  virtual void OnDetach() override;

  virtual void OnUpdate(hammer::Timestep ts) override;
  virtual void OnImGuiRender() override;
  virtual void OnEvent(hammer::Event& e) override;

 private:
  // will use our memory managerment here instead of use hammer::Ref in the
  // future
  hammer::ShaderLibrary shader_library_;
  hammer::Ref<hammer::Shader> shader_;

  hammer::Ref<hammer::VertexArray> vertex_array_;

  hammer::Ref<hammer::Shader> flat_color_shader_;
  hammer::Ref<hammer::VertexArray> square_va_;

  hammer::Ref<hammer::VertexArray> sphere_va_;
  hammer::Ref<hammer::Shader> sphere_shader_;

  hammer::Ref<hammer::Texture2D> texture_, cherno_logo_texture_;

  hammer::OrthographicCameraController camera_controller_;

  glm::vec3 square_color_ = {0.2f, 0.3f, 0.8f};
};