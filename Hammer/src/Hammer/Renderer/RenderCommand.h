#pragma once
#include "Hammer/Renderer/RendererAPI.h"

namespace hammer {
class RenderCommand {
 public:
  static void Init() { kRendererAPI->Init(); }

  static void SetViewport(uint32_t x, uint32_t y, uint32_t width,
                                 uint32_t height) {
    kRendererAPI->SetViewport(x, y, width, height);
  }

  static void SetClearColor(const glm::vec4& color) {
    kRendererAPI->SetClearColor(color);
  }

  static void Clear() { kRendererAPI->Clear(); }

  static void DrawIndexed(const Ref<VertexArray>& vetex_array, uint32_t index_count = 0) {
    kRendererAPI->DrawIndexed(vetex_array, index_count);
  }
  static void DrawLines(const Ref<VertexArray>& vetex_array,
    uint32_t vertext_count = 0) {
    kRendererAPI->DrawLines(vetex_array, vertext_count);
  }

  static void SetLineWidth(float width) { kRendererAPI->SetLineWidth(width); }

 private:
  static Scope<RendererAPI> kRendererAPI;
};

}  // namespace hammer