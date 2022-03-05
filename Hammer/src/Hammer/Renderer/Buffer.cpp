#include "hmpch.h"
#include "Buffer.h"
#include "Hammer/Renderer/Renderer.h"
#include "PlatForm/OpenGL/OpenGLBuffer.h"

namespace hammer {
VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size) {
  switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
      HM_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
      break;
    case RendererAPI::API::OpenGL:
      return new OpenGLVertexBuffer(vertices, size);
      break;
  }
  HM_CORE_ASSERT(false, "Unknown RendererAPI!");
  return nullptr;
}

IndexBuffer* IndexBuffer::Creaet(uint32_t* indices, uint32_t size) {
  switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
      HM_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
      break;
    case RendererAPI::API::OpenGL:
      return new OpenGLIndexBuffer(indices, size);
      break;
  }
  HM_CORE_ASSERT(false, "Unknown RendererAPI!");
  return nullptr;

}
}// namespace hammer