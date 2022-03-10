#pragma once
#include <string>
#include <glm/glm.hpp>
namespace hammer {
class Shader {
 public:
  Shader() = default;
  virtual ~Shader() = default;

  virtual void Bind() const = 0;
  virtual void Unbind() const = 0;
  static Shader* Create(const std::string& vertex_src,
                        const std::string& fragment_src);
};
} // namespace hammer