#include "hmpch.h"
#include "Shader.h"
#include "Renderer.h"
#include "PlatForm/OpenGL/OpenGLShader.h"

namespace hammer {

Shader* Shader::Create(const std::string& vertex_src,
                       const std::string& fragment_src) {
  switch (Renderer::GetAPI()) { 
    case RendererAPI::API::None:
      HM_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
    case RendererAPI::API::OpenGL:
      return new OpenGLShader(vertex_src, fragment_src);
  }
  HM_CORE_ASSERT(false, "Unknow RendererAPI!");
  return nullptr;
}
}  // namespace hammer