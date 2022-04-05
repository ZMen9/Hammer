#include "hmpch.h"
#include "Hammer/Renderer/UniformBuffer.h"
#include "Hammer/Renderer/Renderer.h"
#include "PlatForm/OpenGL/OpenGLUniformBuffer.h"
namespace hammer {
Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding) {
  switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
      HM_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
    case RendererAPI::API::OpenGL:
      return CreateRef<OpenGLUniformBuffer>(size, binding);
  }
  HM_CORE_ASSERT(false, "Unknown RendererAPI!");
  return nullptr;
}
}