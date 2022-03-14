#pragma once
#include <string>
#include <unordered_map>

namespace hammer {
class Shader {
 public:
  Shader() = default;
  virtual ~Shader() = default;

  virtual void Bind() const = 0;
  virtual void Unbind() const = 0;
  static Ref<Shader> Create(const std::string& file_path);
  static Ref<Shader> Create(const std::string& name,
                            const std::string& vertex_src,
                            const std::string& fragment_src);

  virtual const std::string& name() const = 0;
};

class ShaderLibrary {
 public:
  void Add(const std::string& name, const Ref<Shader>& shader);
  void Add(const Ref<Shader>& shader);
  Ref<Shader> Load(const std::string& file_path);
  Ref<Shader> Load(const std::string& name, const std::string& file_path);

  Ref<Shader> Get(const std::string& name);

  bool Exists(const std::string& name) const;
 private:
  std::unordered_map<std::string, Ref<Shader>> shaders_;
};
} // namespace hammer