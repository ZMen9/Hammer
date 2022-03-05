#pragma once
#include "RenderCommand.h"

namespace hammer {

class Renderer {
 public:
  static void BeginScene();
  static void EndScene();
  static void Submit(const std::shared_ptr<VertexArray>& vertex_array);

 public:
  inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
};

}  // namespace hammer