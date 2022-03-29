#include "hmpch.h"
#include "Hammer/Renderer/Texture.h"
#include "Hammer/Renderer/RendererAPI.h"
#include "PlatForm/OpenGL/OpenGLTexture.h"

namespace hammer {
Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t heigth) {
  switch (RendererAPI::GetAPI()) {
    case RendererAPI::API::None:
      HM_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
    case RendererAPI::API::OpenGL:
      return CreateRef<OpenGLTexture2D>(width, heigth);
  }
  HM_CORE_ASSERT(false, "Unknown RendererAPI!");
  return nullptr;
}

Ref<Texture2D> Texture2D::Create(const std::string& path) {
  switch (RendererAPI::GetAPI()) { 
    case RendererAPI::API::None: 
      HM_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
    case RendererAPI::API::OpenGL:
      return CreateRef<OpenGLTexture2D>(path);
  }
  HM_CORE_ASSERT(false, "Unknown RendererAPI!");
  return nullptr;
}
} // namespace hammer