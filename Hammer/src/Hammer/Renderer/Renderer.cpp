#include "hmpch.h"
#include "Hammer/Renderer/Renderer.h"
#include "PlatForm/OpenGL/OpenGLShader.h"
#include "Hammer/Renderer/Renderer2D.h"


namespace hammer {
Scope<Renderer::SceneDate> Renderer::kSceneData =
    CreateScope<Renderer::SceneDate>();

void Renderer::Init() { 
  HM_PROFILE_FUNCTION();
  RenderCommand::Init();
  Renderer2D::Init();
}
void Renderer::Shutdown() { Renderer2D::Shutdown(); } 

void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
  RenderCommand::SetViewport(0, 0, width, height);
}

void Renderer::BeginScene(OrthographicCamera& camera) {
  kSceneData->view_projection_matrix = camera.view_projection_matrix();
}

void Renderer::EndScene() {}

void Renderer::Submit(const Ref<Shader>& shader,
                      const Ref<VertexArray>& vertex_array,
                      const glm::mat4& transform) {
  shader->Bind();
  // didn't use static_cast: for runtime type checking safe 
  //std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", kSceneData->view_projection_matrix);
  shader->SetMat4("u_ViewProjection", kSceneData->view_projection_matrix);
  // transform matrix will be push in the render command queue in the future dev
  //std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);
  shader->SetMat4("u_Transform", transform);
  vertex_array->Bind();
  RenderCommand::DrawIndexed(vertex_array);
}

}  // namespace hammer