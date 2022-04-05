#pragma once

#include "Hammer/Renderer/Framebuffer.h"

namespace hammer {
class OpenGLFramebuffer : public Framebuffer{
 public:
  OpenGLFramebuffer(const FramebufferSpecification& spec);
  virtual ~OpenGLFramebuffer() override;

  void Invalidate();
  virtual void Bind() override;
  virtual void Unbind() override;
  virtual void Resize(uint32_t width, uint32_t height) override;
  virtual int ReadPixel(uint32_t attachment_index, int x, int y) override;
  virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override {
    HM_CORE_ASSERT(index < color_attachments_.size());
    return color_attachments_[index];
  }
  virtual void ClearAttachment(uint32_t attachment_index, int value) override;
  virtual const FramebufferSpecification& GetSpecification() const override {
    return specification_;
  }


 private:
  uint32_t renderer_id_ = 0;
  FramebufferSpecification specification_;
  FramebufferTextureSpecification depth_attachment_specification_ =
      FramebufferTextureFormat::None;
  // color attachment color formats
  std::vector<FramebufferTextureSpecification> color_attachment_specifiactions_;
  // color attachment ids
  std::vector<uint32_t> color_attachments_;
  uint32_t depth_attachment_ = 0;
};

}  // namespace hammer
