#pragma once
#include <string>
#include <glm/glm.hpp>
namespace hammer {
class Shader {
 public:
  Shader(const std::string& vertex_src, const std::string& fragment_src);
  ~Shader();

  void Bind() const;
  void Unbind() const;

  void UploadUniformMat4(const std::string& name, const glm::mat4 matrix);
 private:
  uint32_t render_id_ ;
};
} // namespace hammer