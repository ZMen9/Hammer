#include "hmpch.h"
#include "PlatForm/OpenGL/OpenGLTexture.h"

#include <stb_image.h>

namespace hammer {

OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
    : width_(width), height_(height) {
  HM_PROFILE_FUNCTION();
  internal_format_ = GL_RGBA8;
  data_format_ = GL_RGBA;
  glCreateTextures(GL_TEXTURE_2D, 1, &renderer_id_);
  glTextureStorage2D(renderer_id_, 1, internal_format_, width_, height_);

  glTextureParameteri(renderer_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(renderer_id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_T, GL_REPEAT);

}

OpenGLTexture2D::OpenGLTexture2D(const std::string& path) 
		: path_(path) {
  HM_PROFILE_FUNCTION();
  int width, height, channels;
  stbi_set_flip_vertically_on_load(1);
  stbi_uc* data = nullptr;
  {
    HM_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)"); 
    data = stbi_load(path.c_str(), &width, &height, &channels, 0);
  }
  if (data) {
    width_ = width;
    height_ = height;
    GLenum internal_format = 0, data_format = 0;
    if (channels == 4) {
      internal_format = GL_RGBA8;
      data_format = GL_RGBA;
    } else if (channels == 3) {
      internal_format = GL_RGB8;
      data_format = GL_RGB;
    }
    internal_format_ = internal_format;
    data_format_ = data_format;
    HM_CORE_ASSERT(internal_format & data_format, "Format not supported!");
    glCreateTextures(GL_TEXTURE_2D, 1, &renderer_id_);
    glTextureStorage2D(renderer_id_, 1, internal_format, width_, height_);

    glTextureParameteri(renderer_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(renderer_id_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureSubImage2D(renderer_id_, 0, 0, 0, width_, height_, data_format,
                        GL_UNSIGNED_BYTE, data);
    is_load_ = true;
    stbi_image_free(data);
  }
}

OpenGLTexture2D::~OpenGLTexture2D() {
  HM_PROFILE_FUNCTION();
  glDeleteTextures(1, &renderer_id_);
}


void OpenGLTexture2D::Bind(uint32_t slot/* = 0*/) const {
  HM_PROFILE_FUNCTION();
  glBindTextureUnit(slot, renderer_id_);
}

void OpenGLTexture2D::SetData(void* data, uint32_t size) {
  HM_PROFILE_FUNCTION();
  uint32_t bpp = data_format_ == GL_RGBA ? 4 : 3;
  HM_CORE_ASSERT(size == width_ * height_ * bpp,
                 "Data must be entire texture!");
  glTextureSubImage2D(renderer_id_, 0, 0, 0, width_, height_, data_format_, GL_UNSIGNED_BYTE, data);
  
}

}  // namespace hammer