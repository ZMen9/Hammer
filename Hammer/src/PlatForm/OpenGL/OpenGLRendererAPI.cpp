#include "hmpch.h"
#include "OpenGLRendererAPI.h"
#include <glad/glad.h>

namespace hammer {

void OpenRendererAPI::SetClearColor(const glm::vec4& color) {
  glClearColor(color.r, color.g, color.b, color.a);
}

void OpenRendererAPI::Clear() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenRendererAPI::DrawIndexed(
    const std::shared_ptr<VertexArray>& vetex_array) {
  glDrawElements(GL_TRIANGLES, vetex_array->GetIndexBuffer()->GetCount(),
                 GL_UNSIGNED_INT, nullptr);
}

} // namespace hammer