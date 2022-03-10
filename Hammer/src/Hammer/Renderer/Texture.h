#pragma once
#include <string>

#include "Hammer/Core.h"

namespace hammer {
class Texture {
 public:
  virtual ~Texture() = default;

  virtual uint32_t width() const = 0;
  virtual uint32_t heigth() const = 0;

  virtual void Bind(uint32_t slot = 0) const = 0;
};

class Texture2D : public Texture {
 public:
  static Ref<Texture2D> Create(const std::string& path);
};
}  // namespace hammer