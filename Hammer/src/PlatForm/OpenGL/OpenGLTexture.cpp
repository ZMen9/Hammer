#include "hmpch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>
#include "stb_image.h"

namespace hammer {

OpenGLTexture2D::OpenGLTexture2D(const std::string& path) 
		: path_(path_) {
  int width, height, channels;
  stbi_set_flip_vertically_on_load(1);
  stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
  HM_CORE_ASSERT(data, "Failed to load image!");
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
  HM_CORE_ASSERT(internal_format & data_format, "Format not supported!");

  glCreateTextures(GL_TEXTURE_2D, 1, &renderer_id_);
  glTextureStorage2D(renderer_id_, 1, internal_format, width_, height_);

  glTextureParameteri(renderer_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(renderer_id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTextureSubImage2D(renderer_id_, 0, 0, 0, width_, height_, data_format,
                      GL_UNSIGNED_BYTE, data);

  stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D() { glDeleteTextures(1, &renderer_id_); }


void OpenGLTexture2D::Bind(uint32_t slot/* = 0*/) const {
  glBindTextureUnit(slot, renderer_id_);
}

}  // namespace hammer