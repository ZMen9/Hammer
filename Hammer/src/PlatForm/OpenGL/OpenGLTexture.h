#pragma once
#include "Hammer/Renderer/Texture.h"

namespace hammer {
class OpenGLTexture2D : public Texture2D {
 public:
  OpenGLTexture2D(const std::string& path);
  virtual ~OpenGLTexture2D();

  virtual uint32_t width() const override { return width_; }
  virtual uint32_t heigth() const override { return height_; }
  virtual void Bind(uint32_t slot = 0) const override;

 private:
  std::string path_;
  uint32_t width_, height_;
  uint32_t renderer_id_;
};
}  // namespace hammer