#pragma once
#include "Hammer/Renderer/RenderCommand.h"
#include "Hammer/Renderer/OrthographicCamera.h"
#include "Hammer/Renderer/Shader.h"
namespace hammer {

class Renderer {
 public:
  static void Init();
  static void Shutdown();
  static void OnWindowResize(uint32_t width, uint32_t height);

  static void BeginScene(OrthographicCamera& camera);
  static void EndScene();
  static void Submit(const Ref<Shader>& shader,
                     const Ref<VertexArray>& vertex_array,
                     const glm::mat4& transform = glm::mat4(1.0f));

 public:
  inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

 private:
  struct SceneDate {
    glm::mat4 view_projection_matrix;
  };
  static Scope<SceneDate> kSceneData;
};

}  // namespace hammer