#pragma once
#include "Hammer/Renderer/RendererAPI.h"

namespace hammer {
class OpenGLRendererAPI : public RendererAPI {
 public:
  OpenGLRendererAPI() = default;
  virtual void Init() override;
  virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width,
                           uint32_t height) override;
  virtual void SetClearColor(const glm::vec4& color) override;
  virtual void Clear() override;
  virtual void DrawIndexed(const Ref<VertexArray>& vetex_array, uint32_t index_count = 0) override;
  virtual void DrawLines(const Ref<VertexArray>& vetex_array,
                         uint32_t vertex_count) override;
  virtual void SetLineWidth(float width) override;
};
}// namespace hammer