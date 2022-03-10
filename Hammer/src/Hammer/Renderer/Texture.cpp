#include "hmpch.h"
#include "Texture.h"
#include "Hammer/Renderer/RendererAPI.h"
#include "PlatForm/OpenGL/OpenGLTexture.h"

namespace hammer {
Ref<Texture2D> Texture2D::Create(const std::string& path) {
  switch (RendererAPI::GetAPI()) { 
    case RendererAPI::API::None: 
      HM_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
    case RendererAPI::API::OpenGL:
      return std::make_shared<OpenGLTexture2D>(path);
  }
  HM_CORE_ASSERT(false, "Unknown RendererAPI!");
  return nullptr;
}
} // namespace hammer