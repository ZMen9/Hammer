#include "hmpch.h"
#include "Renderer.h"

namespace hammer {
Renderer::SceneDate* Renderer::kSceneData = new Renderer::SceneDate;

void Renderer::BeginScene(OrthographicCamera& camera) {
  kSceneData->view_projection_matrix = camera.view_projection_matrix();
}

void Renderer::EndScene() {}

void Renderer::Submit(const std::shared_ptr<Shader>& shader,
                      const std::shared_ptr<VertexArray>& vertex_array) {
  shader->Bind();
  shader->UploadUniformMat4("u_ViewProjection", kSceneData->view_projection_matrix);
  vertex_array->Bind();
  RenderCommand::DrawIndexed(vertex_array);
}

}  // namespace hammer