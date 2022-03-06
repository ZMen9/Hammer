#pragma once
#include "RenderCommand.h"
#include "OrthographicCamera.h"
#include "Shader.h"
namespace hammer {

class Renderer {
 public:
  static void BeginScene(OrthographicCamera& camera);
  static void EndScene();
  static void Submit(const std::shared_ptr<Shader>& shader, 
                     const std::shared_ptr<VertexArray>& vertex_array);

 public:
  inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

 private:
  struct SceneDate {
    glm::mat4 view_projection_matrix;
  };
  static SceneDate* kSceneData;
};

}  // namespace hammer