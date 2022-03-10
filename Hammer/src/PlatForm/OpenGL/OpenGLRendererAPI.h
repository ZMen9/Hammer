#pragma once
#include "Hammer/Renderer/RendererAPI.h"

namespace hammer {
class OpenRendererAPI : public RendererAPI {
 public:
  OpenRendererAPI() = default;
  virtual void Init() override;
  virtual void SetClearColor(const glm::vec4& color) override;
  virtual void Clear() override;
  virtual void DrawIndexed(const Ref<VertexArray>& vetex_array) override;
};
}// namespace hammer