#pragma once
#include <glm/glm.hpp>
#include "VertexArray.h"
namespace hammer {
class RendererAPI {
 public:
  enum class API {
    None = 0,
    OpenGL = 1
  };

 public:
  virtual void Init() = 0;
  virtual void SetClearColor(const glm::vec4& color) = 0;
  virtual void Clear() = 0;
  virtual void DrawIndexed(const Ref<VertexArray>& vetex_array) = 0;

  inline static API GetAPI() { return kAPI; };

 private:
  static API kAPI;
};
}  // namespace hammer