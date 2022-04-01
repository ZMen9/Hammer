#include "hmpch.h"
#include "PlatForm/OpenGL/OpenGLFramebuffer.h"
#include <glad/glad.h>

namespace hammer {

static constexpr uint32_t kMaxFrameBufferSize = 8192;

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
    : specification_(spec) {
  Invalidate();
}

OpenGLFramebuffer::~OpenGLFramebuffer() {
  glDeleteFramebuffers(1, &renderer_id_);
  glDeleteTextures(1, &color_attachment_);
  glDeleteTextures(1, &depth_attachment_);
}

void OpenGLFramebuffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER,renderer_id_);
  glViewport(0, 0, specification_.width, specification_.height);
}

void OpenGLFramebuffer::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER,0); }

void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height) {
  if (width == 0 || height == 0 || width > kMaxFrameBufferSize ||
      height > kMaxFrameBufferSize) {
    HM_CORE_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
    return;
  }
  specification_.width = width;
  specification_.height = height;
  Invalidate();
}

void OpenGLFramebuffer::Invalidate() {
  if (renderer_id_) {
    glDeleteFramebuffers(1, &renderer_id_);
    glDeleteTextures(1, &color_attachment_);
    glDeleteTextures(1, &depth_attachment_);
  }

  glCreateFramebuffers(1, &renderer_id_);
  glBindFramebuffer(GL_FRAMEBUFFER,renderer_id_);

	glCreateTextures(GL_TEXTURE_2D, 1, &color_attachment_);
  // don't forget to bind
  glBindTexture(GL_TEXTURE_2D, color_attachment_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, specification_.width,
               specification_.height, 0, GL_RGBA, 
               GL_UNSIGNED_BYTE,nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         color_attachment_, 0);

  glCreateTextures(GL_TEXTURE_2D, 1, &depth_attachment_);
  glBindTexture(GL_TEXTURE_2D, depth_attachment_);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, specification_.width,
                 specification_.height);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
                         depth_attachment_, 0);
  HM_CORE_ASSERT(
     glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
      "Framebuffer is incomplete!");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}  // namespace hammer