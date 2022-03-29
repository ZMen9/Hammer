#include "hmpch.h"
#include "Hammer/Renderer/GraphicsContext.h"
#include "Hammer/Renderer/Renderer.h"
#include "PlatForm/OpenGL/OpenGLContext.h"

namespace hammer {
Scope<GraphicsContext> GraphicsContext::Create(void* window) {
  switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
      HM_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
    case RendererAPI::API::OpenGL:
      return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
  }

  HM_CORE_ASSERT(false, "Unknown RendererAPI!");
  return nullptr;
}

} // namespace hammer