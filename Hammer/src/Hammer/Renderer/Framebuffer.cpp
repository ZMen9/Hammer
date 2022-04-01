#include "hmpch.h"
#include "Hammer/Renderer/Framebuffer.h"
#include "Hammer/Renderer/Renderer.h"
#include "PlatForm/OpenGL/OpenGLFramebuffer.h"

namespace hammer {
Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec) {
  switch (Renderer::GetAPI()) { 
    case RendererAPI::API::None:
      HM_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
    case RendererAPI::API::OpenGL:
      return CreateRef<OpenGLFramebuffer>(spec);
  }
  HM_CORE_ASSERT(false, "Unknown RendererAPI!");
  return nullptr;
}

}