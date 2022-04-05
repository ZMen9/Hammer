#include "hmpch.h"
#include "PlatForm/OpenGL/OpenGLShader.h"
#include "Hammer/Core/Timer.h"
#include <glad/glad.h>

#include <filesystem>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
namespace hammer {
namespace utils {
static GLenum ShaderTypeFromString(const std::string& type) {
  if (type == "vertex")
    return GL_VERTEX_SHADER;
  else if (type == "fragment" || type == "pixel")
    return GL_FRAGMENT_SHADER;
  HM_CORE_ASSERT(false, "Unknown shader type!");
  return 0;
}
static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage) {
  switch (stage) {
    case GL_VERTEX_SHADER:
      return shaderc_glsl_vertex_shader;
    case GL_FRAGMENT_SHADER:
      return shaderc_glsl_fragment_shader;
  }
  HM_CORE_ASSERT(false);
  return (shaderc_shader_kind)0;
}
static const char* GLShaderStageToString(GLenum stage) {
  switch (stage) {
    case GL_VERTEX_SHADER:
      return "GL_VERTEX_SHADER";
    case GL_FRAGMENT_SHADER:
      return "GL_FRAGMENT_SHADER";
  }
  HM_CORE_ASSERT(false);
  return nullptr;
}
static const char* GetCacheDirectory() {
  // TODO: make sure the assets directory is valid
  return "assets/cache/shader/opengl";
}
static void CreateCacheDirectoryIfNeeded() {
  std::string cacheDirectory = GetCacheDirectory();
  if (!std::filesystem::exists(cacheDirectory))
    std::filesystem::create_directories(cacheDirectory);
}
static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage) {
  switch (stage) {
    case GL_VERTEX_SHADER:
      return ".cached_opengl.vert";
    case GL_FRAGMENT_SHADER:
      return ".cached_opengl.frag";
  }
  HM_CORE_ASSERT(false);
  return "";
}
static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage) {
  switch (stage) {
    case GL_VERTEX_SHADER:
      return ".cached_vulkan.vert";
    case GL_FRAGMENT_SHADER:
      return ".cached_vulkan.frag";
  }
  HM_CORE_ASSERT(false);
  return "";
}
} // utils

OpenGLShader::OpenGLShader(const std::string& file_path)
    : file_path_(file_path){
  HM_PROFILE_FUNCTION();
  utils::CreateCacheDirectoryIfNeeded();
  std::string source = ReadFile(file_path);
  auto shader_sources = PreProcess(source);
  {
    Timer timer;
    CompileOrGetVulkanBinaries(shader_sources);
    CompileOrGetOpenGLBinaries();
    CreateProgram();
    HM_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
  }

  // Extract name from filepath
  /*auto lastSlash = file_path.find_last_of("/\\");
  lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
  auto lastDot = file_path.rfind('.');
  auto count = lastDot == std::string::npos ? file_path.size() - lastSlash
                                            : lastDot - lastSlash;
  shader_name_ = file_path.substr(lastSlash, count);*/
  std::filesystem::path path = file_path;
  shader_name_ = path.stem().string();
}

OpenGLShader::OpenGLShader(const std::string& name,
                           const std::string& vertex_src,
                           const std::string& fragment_src)
    : shader_name_(name) {
  HM_PROFILE_FUNCTION();
  std::unordered_map<GLenum, std::string> sources;
  sources[GL_VERTEX_SHADER] = vertex_src;
  sources[GL_FRAGMENT_SHADER] = fragment_src;

  CompileOrGetVulkanBinaries(sources);
  CompileOrGetOpenGLBinaries();
  CreateProgram();
}

OpenGLShader::~OpenGLShader() {
  HM_PROFILE_FUNCTION();
  glDeleteProgram(renderer_id_);
}

void OpenGLShader::Bind() const {
  HM_PROFILE_FUNCTION();
  glUseProgram(renderer_id_);
}

void OpenGLShader::Unbind() const {
  HM_PROFILE_FUNCTION();
  glUseProgram(0);
}

void OpenGLShader::SetInt(const std::string& name, int value) {
  HM_PROFILE_FUNCTION();
  UploadUniformInt(name, value);
}

void OpenGLShader::SetIntArray(const std::string& name, int* values,
                               uint32_t count) {
  HM_PROFILE_FUNCTION();
  UploatUniformIntArray(name, values, count);
}
void OpenGLShader::SetFloat(const std::string& name, float value) {
  HM_PROFILE_FUNCTION();
  UploadUniformFloat(name, value);
}
void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value) {
  HM_PROFILE_FUNCTION();
  UploadUniformFloat2(name, value);
}

void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value) {
  HM_PROFILE_FUNCTION();
  UploadUniformFloat3(name, value);
}

void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value) {
  HM_PROFILE_FUNCTION();
  UploadUniformFloat4(name, value);
}

void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value) {
  HM_PROFILE_FUNCTION();
  UploadUniformMat4(name, value);
}

void OpenGLShader::UploadUniformInt(const std::string& name, int value) {
  HM_PROFILE_FUNCTION();
  GLint location = glGetUniformLocation(renderer_id_, name.c_str());
  glUniform1i(location, value);
}

void OpenGLShader::UploatUniformIntArray(const std::string& name, int* values,
                                         uint32_t count) {
  HM_PROFILE_FUNCTION();
  GLint location = glGetUniformLocation(renderer_id_, name.c_str());
  glUniform1iv(location, count, values);
}

void OpenGLShader::UploadUniformFloat(const std::string& name, float value) {
  HM_PROFILE_FUNCTION();
  GLint location = glGetUniformLocation(renderer_id_, name.c_str());
  glUniform1f(location, value);
}

void OpenGLShader::UploadUniformFloat2(const std::string& name,
                                       const glm::vec2& value) {
  HM_PROFILE_FUNCTION();
  GLint location = glGetUniformLocation(renderer_id_, name.c_str());
  glUniform2f(location, value.x, value.y);
}

void OpenGLShader::UploadUniformFloat3(const std::string& name,
                                       const glm::vec3& value) {
  HM_PROFILE_FUNCTION();
  GLint location = glGetUniformLocation(renderer_id_, name.c_str());
  glUniform3f(location, value.x, value.y, value.z);
}

void OpenGLShader::UploadUniformFloat4(const std::string& name,
                                       const glm::vec4& value) {
  HM_PROFILE_FUNCTION();
  GLint location = glGetUniformLocation(renderer_id_, name.c_str());
  glUniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGLShader::UploadUniformMat3(const std::string& name,
                                     const glm::mat3& matrix) {
  HM_PROFILE_FUNCTION();
  GLint location = glGetUniformLocation(renderer_id_, name.c_str());
  glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLShader::UploadUniformMat4(const std::string& name,
                                     const glm::mat4& matrix) {
  HM_PROFILE_FUNCTION();
  GLint location = glGetUniformLocation(renderer_id_, name.c_str());
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

std::string OpenGLShader::ReadFile(const std::string& file_path) {
  HM_PROFILE_FUNCTION();
  std::string result;
  std::ifstream in(file_path, std::ios::in | std::ios::binary);// ifstream closes itself due to RAII
  if (in) {
      in.seekg(0, std::ios::end);
      size_t size = in.tellg();
      if (size != -1) {
        result.resize(size);
        in.seekg(0, std::ios::beg);
        in.read(&result[0], size);
        in.close();
      } else {
        HM_CORE_ERROR("Could not open file '{0}' ", file_path);
      }      
  } else {
    HM_CORE_ERROR("Could not open file '{0}' ", file_path);
  }
  return result;
}

std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(
    const std::string& source) {
  HM_PROFILE_FUNCTION();
  std::unordered_map<GLenum, std::string> shader_sources;
  const char* type_token = "#type";
  size_t type_token_length = strlen(type_token);
  size_t pos = source.find(type_token, 0);
  while (pos != std::string::npos) {
    size_t eol = source.find_first_of("\r\n", pos);
    HM_CORE_ASSERT(eol != std::string::npos, "Syntax error");
    size_t begin = pos + type_token_length + 1;
    std::string type = source.substr(begin, eol - begin);
    HM_CORE_ASSERT(utils::ShaderTypeFromString(type), "Invalid shader type specified");
    size_t next_line_pos = source.find_first_not_of("\r\n", eol);
    HM_CORE_ASSERT(next_line_pos != std::string::npos, "Syntax error");
    pos = source.find(type_token, next_line_pos);
    shader_sources[utils::ShaderTypeFromString(type)] =
        (pos == std::string::npos)
            ? source.substr(next_line_pos)
            : source.substr(next_line_pos, pos - next_line_pos);
  }
  return shader_sources;
}

void OpenGLShader::CompileOrGetVulkanBinaries(
    const std::unordered_map<unsigned int, std::string>& shader_sources) {
  HM_PROFILE_FUNCTION();
  GLuint program = glCreateProgram();
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;
  options.SetTargetEnvironment(shaderc_target_env_vulkan,
                               shaderc_env_version_vulkan_1_2);
  const bool optimize = true;
  if (optimize)
    options.SetOptimizationLevel(shaderc_optimization_level_performance);

  std::filesystem::path cache_directory = utils::GetCacheDirectory();
  auto& shader_data = vulkan_spirv_;
  shader_data.clear();
  for (auto&&[stage, source] : shader_sources) {
    std::filesystem::path shader_file_path = file_path_;
    std::filesystem::path cache_path =
        cache_directory /
        (shader_file_path.filename().string() +
         utils::GLShaderStageCachedVulkanFileExtension(stage));
    std::ifstream in(cache_path, std::ios::in | std::ios::binary);
    if (in.is_open()) {
      // GetVulkanBinaries
      in.seekg(0, std::ios::end);
      auto size = in.tellg();
      in.seekg(0, std::ios::beg);
      auto& data = shader_data[stage];
      data.resize(size / sizeof(uint32_t));
      in.read((char*)data.data(), size);
      in.close();
    } else {
      // Compile
      shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
          source, utils::GLShaderStageToShaderC(stage), file_path_.c_str(),
          options);  
      if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
        HM_CORE_ERROR(module.GetErrorMessage());
        HM_CORE_ASSERT(false);
      }
      // Cache
      shader_data[stage] =
          std::vector<uint32_t>(module.cbegin(), module.cend());
      std::ofstream out(cache_path, std::ios::out | std::ios::binary);
      if (out.is_open()) {
        auto& data = shader_data[stage];
        out.write((char*)data.data(), data.size() * sizeof(uint32_t));
        out.flush();
        out.close();
      }
    }
  }
  for (auto&& [stage, data] : shader_data) Reflect(stage, data);
}

void OpenGLShader::CompileOrGetOpenGLBinaries() {
  HM_PROFILE_FUNCTION();
  auto& shader_data = opengl_spirv_;
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;
  options.SetTargetEnvironment(shaderc_target_env_opengl,
                               shaderc_env_version_opengl_4_5);
  const bool optimize = false;
  if (optimize)
    options.SetOptimizationLevel(shaderc_optimization_level_performance);

  std::filesystem::path cache_directory = utils::GetCacheDirectory();
  
  shader_data.clear();
  opengl_source_code_.clear();
  for (auto&& [stage, spirv] : vulkan_spirv_) {
    std::filesystem::path shader_file_path = file_path_;
    std::filesystem::path cache_path =
        cache_directory /
        (shader_file_path.filename().string() +
         utils::GLShaderStageCachedOpenGLFileExtension(stage));
    std::ifstream in(cache_path, std::ios::in | std::ios::binary);
    if (in.is_open()) {
      // GetOpenglBinaries
      in.seekg(0, std::ios::end);
      auto size = in.tellg();
      in.seekg(0, std::ios::beg);
      auto& data = shader_data[stage];
      data.resize(size / sizeof(uint32_t));
      in.read((char*)data.data(), size);
      in.close();
    } else {
      // Cross Compile
      spirv_cross::CompilerGLSL glslComplier(spirv);
      opengl_source_code_[stage] = glslComplier.compile();
      auto& source = opengl_source_code_[stage];
      shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
          source, utils::GLShaderStageToShaderC(stage), file_path_.c_str());
      if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
        HM_CORE_ERROR(module.GetErrorMessage());
        HM_CORE_ASSERT(false);
      }
      // Cache
      shader_data[stage] =
          std::vector<uint32_t>(module.cbegin(), module.cend());
      std::ofstream out(cache_path, std::ios::out | std::ios::binary);
      if (out.is_open()) {
        auto& data = shader_data[stage];
        out.write((char*)data.data(), data.size() * sizeof(uint32_t));
        out.flush();
        out.close();
      }
    }
  }
}

void OpenGLShader::CreateProgram() {
  HM_PROFILE_FUNCTION();
  GLuint program = glCreateProgram();
  std::vector<GLuint> shader_ids;
  for (auto&& [stage, spirv] : opengl_spirv_) {
    GLuint shader_id = shader_ids.emplace_back(glCreateShader(stage));
    glShaderBinary(1, &shader_id, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(),
                   spirv.size() * sizeof(uint32_t));
    glSpecializeShader(shader_id, "main", 0, nullptr, nullptr);
    glAttachShader(program, shader_id);
  }
  glLinkProgram(program);
  GLint isLinked = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
  if (isLinked == GL_FALSE) {
    GLint maxLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    // The maxLength includes the NULL character
    std::vector<GLchar> infoLog(maxLength);
    glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
    // We don't need the program anymore.
    glDeleteProgram(program);
  }
  for (auto id : shader_ids) {
    glDetachShader(program, id);
    glDeleteShader(id);
  }
  renderer_id_ = program;
}

void OpenGLShader::Reflect(GLenum stage,
                           const std::vector<uint32_t>& shader_data) {
  HM_PROFILE_FUNCTION();
  spirv_cross::Compiler compiler(shader_data);
  spirv_cross::ShaderResources resources = compiler.get_shader_resources();
  HM_CORE_TRACE("OpenGLShader::Reflect - {0} {1}",
                utils::GLShaderStageToString(stage), file_path_);
  HM_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
  HM_CORE_TRACE("    {0} resources", resources.sampled_images.size());

  HM_CORE_TRACE("Uniform buffers:");
  for (const auto& resource : resources.uniform_buffers) {
    const auto& bufferType = compiler.get_type(resource.base_type_id);
    uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
    uint32_t binding =
        compiler.get_decoration(resource.id, spv::DecorationBinding);
    int memberCount = bufferType.member_types.size();

    HM_CORE_TRACE("  {0}", resource.name);
    HM_CORE_TRACE("    Size = {0}", bufferSize);
    HM_CORE_TRACE("    Binding = {0}", binding);
    HM_CORE_TRACE("    Members = {0}", memberCount);
  }
}

}  // namespace hammer
