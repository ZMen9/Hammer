#include "hmpch.h"
#include "PlatForm/OpenGL/OpenGLRendererAPI.h"

#include <glad/glad.h>


namespace hammer {
void OpenGLMessageCallback(unsigned source, unsigned type, unsigned id,
                           unsigned severity, int length, const char* message,
                           const void* userParam) {
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      HM_CORE_CRITICAL(message);
      return;
    case GL_DEBUG_SEVERITY_MEDIUM:
      HM_CORE_ERROR(message);
      return;
    case GL_DEBUG_SEVERITY_LOW:
      HM_CORE_WARN(message);
      return;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      HM_CORE_TRACE(message);
      return;
  }

  HM_CORE_ASSERT(false, "Unknown severity level!");
}
void OpenGLRendererAPI::Init() {
  HM_PROFILE_FUNCTION();

  #ifdef  HM_DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(OpenGLMessageCallback, nullptr);

  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
                        GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
  #endif  // HM_DEBUG

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  // glDepthFunc(GL_LESS);
}

void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width,
                                  uint32_t height) {
  glViewport(x, y, width, height);
}

void OpenGLRendererAPI::SetClearColor(const glm::vec4& color) {
  glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRendererAPI::Clear() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vetex_array, uint32_t index_count) {
  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_BACK);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  uint32_t count =
      index_count ? index_count : vetex_array->GetIndexBuffer()->GetCount() ;
  glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);
}

}  // namespace hammer