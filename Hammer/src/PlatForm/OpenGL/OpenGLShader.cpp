#include "hmpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>

#include <filesystem>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>


namespace hammer {
static GLenum ShaderTypeFromString(const std::string& type) {
  if (type == "vertex")
    return GL_VERTEX_SHADER;
  else if (type == "fragment" || type == "pixel")
    return GL_FRAGMENT_SHADER;
  HM_CORE_ASSERT(false, "Unknown shader type!");
  return 0;
}

OpenGLShader::OpenGLShader(const std::string& file_path) {
  std::string source = ReadFile(file_path);
  auto shader_sources = PreProcess(source);
  Compile(shader_sources);

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
  std::unordered_map<GLenum, std::string> sources;
  sources[GL_VERTEX_SHADER] = vertex_src;
  sources[GL_FRAGMENT_SHADER] = fragment_src;
  Compile(sources);
}

OpenGLShader::~OpenGLShader() { glDeleteProgram(renderer_id_); }

void OpenGLShader::Bind() const { glUseProgram(renderer_id_); }

void OpenGLShader::Unbind() const { glUseProgram(0); }

void OpenGLShader::UploadUniformInt(const std::string& name, int value) {
  GLint location = glGetUniformLocation(renderer_id_, name.c_str());
  glUniform1i(location, value);
}

void OpenGLShader::UploadUniformFloat(const std::string& name, float value) {
  GLint location = glGetUniformLocation(renderer_id_, name.c_str());
  glUniform1f(location, value);
}

void OpenGLShader::UploadUniformFloat2(const std::string& name,
                                       const glm::vec2& value) {
  GLint location = glGetUniformLocation(renderer_id_, name.c_str());
  glUniform2f(location, value.x, value.y);
}

void OpenGLShader::UploadUniformFloat3(const std::string& name,
                                       const glm::vec3& value) {
  GLint location = glGetUniformLocation(renderer_id_, name.c_str());
  glUniform3f(location, value.x, value.y, value.z);
}

void OpenGLShader::UploadUniformFloat4(const std::string& name,
                                       const glm::vec4& value) {
  GLint location = glGetUniformLocation(renderer_id_, name.c_str());
  glUniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGLShader::UploadUniformMat3(const std::string& name,
                                     const glm::mat3& matrix) {
  GLint location = glGetUniformLocation(renderer_id_, name.c_str());
  glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLShader::UploadUniformMat4(const std::string& name,
                                     const glm::mat4& matrix) {
  GLint location = glGetUniformLocation(renderer_id_, name.c_str());
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

std::string OpenGLShader::ReadFile(const std::string& file_path) {
  std::string result;
  std::ifstream in(file_path, std::ios::in | std::ios::binary);
  if (in) {
    in.seekg(0, std::ios::end);
    result.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&result[0], result.size());
    in.close();
  } else {
    HM_CORE_ERROR("Could not open file '{0}' ", file_path);
  }
  return result;
}

std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(
    const std::string& source) {
  std::unordered_map<GLenum, std::string> shader_sources;
  const char* type_token = "#type";
  size_t type_token_length = strlen(type_token);
  size_t pos = source.find(type_token, 0);
  while (pos != std::string::npos) {
    size_t eol = source.find_first_of("\r\n", pos);
    HM_CORE_ASSERT(eol != std::string::npos, "Syntax error");
    size_t begin = pos + type_token_length + 1;
    std::string type = source.substr(begin, eol - begin);
    HM_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");
    size_t next_line_pos = source.find_first_not_of("\r\n", eol);
    pos = source.find(type_token, next_line_pos);
    shader_sources[ShaderTypeFromString(type)] =
        source.substr(next_line_pos, pos - (next_line_pos == std::string::npos
                                                ? source.size() - 1
                                                : next_line_pos));
  }
  return shader_sources;
}

void OpenGLShader::Compile(
    const std::unordered_map<GLenum, std::string>& shaderSources) {
  GLuint program = glCreateProgram();
  HM_CORE_ASSERT(shaderSources.size() <= 2,
                 "We only support 2 shaders for now");
  std::array<GLenum, 2> gl_shader_IDs;
  int gl_shader_ID_idx = 0;
  for (auto& kv : shaderSources) {
    GLenum type = kv.first;
    const std::string& source = kv.second;
    GLuint shader = glCreateShader(type);
    const GLchar* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, 0);

    glCompileShader(shader);
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
      GLint maxLength = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
      std::vector<GLchar> infoLog(maxLength);
      glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
      glDeleteShader(shader);
      HM_CORE_ERROR("{0}", infoLog.data());
      HM_CORE_ASSERT(false, "Shader compilation failure!");
      break;
    }
    glAttachShader(program, shader);
    gl_shader_IDs[gl_shader_ID_idx++] = shader;
  }

  renderer_id_ = program;
  // Link our program
  glLinkProgram(program);
  // Note the different functions here: glGetProgram* instead of glGetShader*.
  GLint isLinked = 0;
  glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
  if (isLinked == GL_FALSE) {
    GLint maxLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    std::vector<GLchar> infoLog(maxLength);
    glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

    // We don't need the program anymore.
    glDeleteProgram(program);

    for (auto id : gl_shader_IDs) glDeleteShader(id);

    HM_CORE_ERROR("{0}", infoLog.data());
    HM_CORE_ASSERT(false, "Shader link failure!");
    return;
  }
  for (auto id : gl_shader_IDs) {
    glDetachShader(program, id);
    glDeleteShader(id);
  
  }
}

}  // namespace hammer