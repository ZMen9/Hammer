#include "hmpch.h"
#include "Hammer/Renderer/Buffer.h"
#include "Hammer/Renderer/Renderer.h"
#include "PlatForm/OpenGL/OpenGLBuffer.h"

namespace hammer {

Ref<VertexBuffer> VertexBuffer::Create(uint32_t size) {
  switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
      HM_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
      break;
    case RendererAPI::API::OpenGL:
      return CreateRef<OpenGLVertexBuffer>(size);
      break;
  }
  HM_CORE_ASSERT(false, "Unknown RendererAPI!");
  return nullptr;
}



Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size) {
  switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
      HM_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
      break;
    case RendererAPI::API::OpenGL:
      return CreateRef<OpenGLVertexBuffer>(vertices, size);
      break;
  }
  HM_CORE_ASSERT(false, "Unknown RendererAPI!");
  return nullptr;
}

Ref<IndexBuffer> IndexBuffer::Creaet(uint32_t* indices, uint32_t size) {
  switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
      HM_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
      break;
    case RendererAPI::API::OpenGL:
      return CreateRef<OpenGLIndexBuffer>(indices, size);
      break;
  }
  HM_CORE_ASSERT(false, "Unknown RendererAPI!");
  return nullptr;

}
}// namespace hammer