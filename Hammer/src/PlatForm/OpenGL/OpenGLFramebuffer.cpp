#include "hmpch.h"
#include "PlatForm/OpenGL/OpenGLFramebuffer.h"
#include <glad/glad.h>

namespace hammer {

static constexpr uint32_t kMaxFrameBufferSize = 8192;

namespace utils {

static GLenum TextureTarget(bool multisampled) {
  return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count) {
  glCreateTextures(TextureTarget(multisampled), count, outID);
}

static void BindTexture(bool multisampled, uint32_t id) {
  glBindTexture(TextureTarget(multisampled), id);
}

static void AttachColorTexture(uint32_t id, int samples, GLenum internal_format,
                               GLenum format, uint32_t width, uint32_t height, 
                               int index) {
  bool multisampled = samples > 1;
  if (multisampled) {
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format, 
                            width,height, GL_FALSE);
  } else {
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
                         TextureTarget(multisampled), id, 0);
}

static void AttachDepthTexture(uint32_t id, int samples, GLenum format,
                               GLenum attachmentType, uint32_t width,
                               uint32_t height) {
  bool multisampled = samples > 1;
  if (multisampled) {
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width,
                            height, GL_FALSE);
  } else {
    glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }

  glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType,
                         TextureTarget(multisampled), id, 0);
}

static bool IsDepthFormat(FramebufferTextureFormat format) {
  switch (format) {
    case FramebufferTextureFormat::DEPTH24STENCIL8:
      return true;
  }

  return false;
}

}  // namespace utils

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
    : specification_(spec) {
  for (auto spec : specification_.attachments.attachments) {
    if (!utils::IsDepthFormat(spec.texture_format))
      color_attachment_specifiactions_.emplace_back(spec);
    else
      depth_attachment_specification_ = spec;
  }
  Invalidate();
}

OpenGLFramebuffer::~OpenGLFramebuffer() {
  glDeleteFramebuffers(1, &renderer_id_);
  glDeleteTextures(color_attachments_.size(), color_attachments_.data());
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

int OpenGLFramebuffer::ReadPixel(uint32_t attachment_index, int x, int y) {
  HM_CORE_ASSERT(attachment_index < color_attachments_.size());
  glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);
  int pixel_data;
  // read one single pixel
  // GL_RED_INTEGER and GL_INT is hard-coded, will be changed
  glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel_data);
  return pixel_data;
}

static GLenum HammerFBTextureFormatTOGL(FramebufferTextureFormat format) {
  switch (format) {
    case hammer::FramebufferTextureFormat::RGBA8:
      return GL_RGBA8;
      break;
    case hammer::FramebufferTextureFormat::RED_INTEGER:
      return GL_RED_INTEGER;
      break;
  }
  HM_CORE_ASSERT(false);
  return 0;
}

void OpenGLFramebuffer::ClearAttachment(uint32_t attachment_index, int value) {
  HM_CORE_ASSERT(attachment_index < color_attachments_.size());

  auto& spec = color_attachment_specifiactions_[attachment_index];
  glClearTexImage(color_attachments_[attachment_index], 0,
                  HammerFBTextureFormatTOGL(spec.texture_format),GL_INT, &value);
}

void OpenGLFramebuffer::Invalidate() {
  if (renderer_id_) {
    glDeleteFramebuffers(1, &renderer_id_);
    glDeleteTextures(color_attachments_.size(), color_attachments_.data());
    glDeleteTextures(1, &depth_attachment_);

    color_attachments_.clear();
    depth_attachment_ = 0;
  }

  glCreateFramebuffers(1, &renderer_id_);
  glBindFramebuffer(GL_FRAMEBUFFER,renderer_id_);

	bool multisample = specification_.samples > 1;

  // Attachments:
  // Color Attachments
  if (color_attachment_specifiactions_.size()) {
    color_attachments_.resize(color_attachment_specifiactions_.size());
    utils::CreateTextures(multisample, color_attachments_.data(),
                          color_attachments_.size());

    for (size_t i = 0; i < color_attachments_.size(); ++i) {
      utils::BindTexture(multisample, color_attachments_[i]);
      switch (color_attachment_specifiactions_[i].texture_format) {
        case FramebufferTextureFormat::RGBA8:
          utils::AttachColorTexture(
              color_attachments_[i], specification_.samples, GL_RGBA8,
              GL_RGBA,specification_.width, specification_.height, i);
          break;
        case FramebufferTextureFormat::RED_INTEGER:
          utils::AttachColorTexture(
              color_attachments_[i], specification_.samples, GL_R32I,
              GL_RED_INTEGER, specification_.width, specification_.height, i);
          break;
      }
    }
  }
  // Depth Attachments
  if (depth_attachment_specification_.texture_format !=
      FramebufferTextureFormat::None) {
    utils::CreateTextures(multisample, &depth_attachment_, 1);
    utils::BindTexture(multisample, depth_attachment_);
    switch (depth_attachment_specification_.texture_format) {
      case FramebufferTextureFormat::DEPTH24STENCIL8:
        utils::AttachDepthTexture(depth_attachment_, specification_.samples,
                                  GL_DEPTH24_STENCIL8,
                                  GL_DEPTH_STENCIL_ATTACHMENT,
                                  specification_.width, specification_.height);
        break;
    }
  }
  // limit the size of attachments
  if (color_attachments_.size() > 1) {
    HM_CORE_ASSERT(color_attachments_.size() <= 4);
    GLenum buffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                         GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    // glDrawBuffers define an array of buffers into which outputs from the
    // fragment shader data will be written. If a fragment shader writes a value
    // to one or more user defined output variables, then the value of each
    // variable will be written into the buffer specified at a location within
    // bufs corresponding to the location assigned to that user defined output.
    // The draw buffer used for user defined outputs assigned to locations
    // greater than or equal to n is implicitly set to GL_NONE and any data
    // written to such an output is discarded.
    glDrawBuffers(color_attachments_.size(), buffers);
  } else if (color_attachments_.empty()) {
    // Only depth-pass
    glDrawBuffer(GL_NONE);
  }
}

}  // namespace hammer