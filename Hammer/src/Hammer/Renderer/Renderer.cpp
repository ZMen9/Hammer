#include "hmpch.h"
#include "Renderer.h"
#include "PlatForm/OpenGL/OpenGLShader.h"


namespace hammer {
Renderer::SceneDate* Renderer::kSceneData = new Renderer::SceneDate;

void Renderer::Init() { RenderCommand::Init(); }

void Renderer::BeginScene(OrthographicCamera& camera) {
  kSceneData->view_projection_matrix = camera.view_projection_matrix();
}

void Renderer::EndScene() {}

void Renderer::Submit(const Ref<Shader>& shader,
                      const Ref<VertexArray>& vertex_array,
                      const glm::mat4& transform) {
  shader->Bind();
  // didn't use static_cast: for runtime type checking safe 
  std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", kSceneData->view_projection_matrix);
  // transform matrix will be push in the render command queue in the future dev
  std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);
  vertex_array->Bind();
  RenderCommand::DrawIndexed(vertex_array);
}

}  // namespace hammer