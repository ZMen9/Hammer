#pragma once

#include "Hammer/Renderer/Shader.h"
#include <glm/glm.hpp>

namespace hammer {
class OpenGLShader : public Shader {
 public:
  OpenGLShader(const std::string& vertex_src, const std::string& fragment_src);
  virtual ~OpenGLShader();
  virtual void Bind() const override;
  virtual void Unbind() const override;
  
  void UploadUniformInt(const std::string& name, int value);
  void UploadUniformFloat(const std::string& name,float value);
  void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
  void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
  void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

  void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
  void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
 private:
  uint32_t renderer_id_;
};
} // namespace hammer
