#include "hmpch.h"
#include "OpenGLRendererAPI.h"
#include <glad/glad.h>

namespace hammer {
void OpenRendererAPI::Init() { 
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glEnable(GL_DEPTH_TEST);
  //glDepthFunc(GL_LESS);
}

void OpenRendererAPI::SetClearColor(const glm::vec4& color) {
  glClearColor(color.r, color.g, color.b, color.a);
}

void OpenRendererAPI::Clear() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenRendererAPI::DrawIndexed(const Ref<VertexArray>& vetex_array) {
  //glEnable(GL_CULL_FACE);
  //glCullFace(GL_BACK);
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDrawElements(GL_TRIANGLES, vetex_array->GetIndexBuffer()->GetCount(),
                 GL_UNSIGNED_INT, nullptr);
}

} // namespace hammer