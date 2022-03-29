#include "hmpch.h"
#include "Hammer/Renderer/RendererAPI.h"
#include "PlatForm/OpenGL/OpenGLRendererAPI.h"

namespace hammer {
RendererAPI::API RendererAPI::kAPI = RendererAPI::API::OpenGL;

Scope<RendererAPI> RendererAPI::Create() {
  switch (kAPI) {
    case RendererAPI::API::None:
      HM_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
    case RendererAPI::API::OpenGL:
      return CreateScope<OpenGLRendererAPI>();
  }

  HM_CORE_ASSERT(false, "Unknown RendererAPI!");
  return nullptr;
}
} // namespace hammer