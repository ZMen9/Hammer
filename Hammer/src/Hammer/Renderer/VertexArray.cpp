#include "hmpch.h"
#include "VertexArray.h"
#include "Renderer.h"

#include "PlatForm/OpenGL/OpenGLVertexArray.h"

namespace hammer {
VertexArray* VertexArray::Create() {
  switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
      HM_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
      break;
    case RendererAPI::API::OpenGL:
      return new OpenGLVertexArray();
      break;
  }
  HM_CORE_ASSERT(false, "Unknown RendererAPI!");
  return nullptr;
}
} //namespace hammer