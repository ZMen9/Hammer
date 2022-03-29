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

  static void DrawIndexed(const Ref<VertexArray>& vetex_array, uint32_t count = 0) {
    kRendererAPI->DrawIndexed(vetex_array, count);
  }

 private:
  static Scope<RendererAPI> kRendererAPI;
};

}  // namespace hammer