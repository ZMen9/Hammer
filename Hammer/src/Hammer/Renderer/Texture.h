#pragma once
#include <string>

#include "Hammer/Core/Base.h"

namespace hammer {
class Texture {
 public:
  virtual ~Texture() = default;

  virtual uint32_t width() const = 0;
  virtual uint32_t heigth() const = 0;

  virtual void Bind(uint32_t slot = 0) const = 0;
  virtual void SetData(void* data, uint32_t size) = 0;
  virtual bool IsLoaded() const = 0;
  virtual uint32_t GetRenderID() const = 0;
  virtual bool operator==(const Texture& other) const = 0;
};

class Texture2D : public Texture {
 public:
  static Ref<Texture2D> Create(const std::string& path);
  static Ref<Texture2D> Create(uint32_t width, uint32_t heigth);
};
}  // namespace hammer