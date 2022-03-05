#pragma once
#include "RendererAPI.h"

namespace hammer {
class RenderCommand {
 public:
  inline static void SetClearColor(const glm::vec4& color) {
    kRendererAPI->SetClearColor(color);
  }

  inline static void Clear() { kRendererAPI->Clear(); }

  inline static void DrawIndexed(
      const std::shared_ptr<VertexArray>& vetex_array) {
    kRendererAPI->DrawIndexed(vetex_array);
  }

 private:
  static RendererAPI* kRendererAPI;
};

}  // namespace hammer