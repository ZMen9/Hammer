#pragma once
#include "Hammer/Renderer/Texture.h"
#include <glad/glad.h>

namespace hammer {
class OpenGLTexture2D : public Texture2D {
 public:
  OpenGLTexture2D(uint32_t width, uint32_t height);
  OpenGLTexture2D(const std::string& path);
  virtual ~OpenGLTexture2D();

  virtual uint32_t width() const override { return width_; }
  virtual uint32_t heigth() const override { return height_; }
  virtual void Bind(uint32_t slot = 0) const override;
  virtual void SetData(void* data, uint32_t size) override;
  virtual bool IsLoaded() const override { return is_load_; }
  virtual uint32_t GetRenderID() const override { return renderer_id_; }

  virtual bool operator==(const Texture& other) const override {
    return renderer_id_ ==
           (dynamic_cast<const OpenGLTexture2D&>(other)).renderer_id_;
  }
 private:
  std::string path_;
  uint32_t width_, height_;
  uint32_t renderer_id_;
  GLenum internal_format_, data_format_;
  bool is_load_ = false;
};
}  // namespace hammer