#include "hmpch.h"
#include "Shader.h"
#include "Renderer.h"
#include "PlatForm/OpenGL/OpenGLShader.h"

namespace hammer {
Ref<Shader> Shader::Create(const std::string& file_path) {
  switch(Renderer::GetAPI()) {
    case RendererAPI::API::None:
      HM_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
    case RendererAPI::API::OpenGL:
      return std::make_shared<OpenGLShader>(file_path);
  }
  HM_CORE_ASSERT(false, "Unknow RendererAPI!");
  return nullptr;
}

Ref<Shader> Shader::Create(const std::string& name,
                           const std::string& vertex_src,
                           const std::string& fragment_src) {
  switch (Renderer::GetAPI()) { 
    case RendererAPI::API::None:
      HM_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
      return nullptr;
    case RendererAPI::API::OpenGL:
      return std::make_shared<OpenGLShader>(name, vertex_src, fragment_src);
  }
  HM_CORE_ASSERT(false, "Unknow RendererAPI!");
  return nullptr;
}

void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader) {
  HM_CORE_ASSERT(!Exists(name), "Shader already exists!");
  shaders_[name] = shader;
}

void ShaderLibrary::Add(const Ref<Shader>& shader) {
  auto& name = shader->name();
  Add(name, shader);
}

Ref<Shader> ShaderLibrary::Load(const std::string& file_path) {
  auto shader = Shader::Create(file_path);
  Add(shader);
  return shader;
}

Ref<Shader> ShaderLibrary::Load(const std::string& name,
                                const std::string& file_path) {
  auto shader = Shader::Create(file_path);
  Add(name, shader);
  return shader;
}

Ref<Shader> ShaderLibrary::Get(const std::string& name) {
  HM_CORE_ASSERT(Exists(name), "Shader not found!");
  return shaders_[name];
}

bool ShaderLibrary::Exists(const std::string& name) const {
  return shaders_.find(name) != shaders_.end();
}

}  // namespace hammer