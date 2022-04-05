#pragma once
#include "Hammer/Core/Base.h"
namespace hammer {
enum class FramebufferTextureFormat {
  None = 0,

  // Color
  RGBA8,
  RED_INTEGER,
  // Depth/stencil
  DEPTH24STENCIL8,

  // Defaults
  Depth = DEPTH24STENCIL8
};

struct FramebufferTextureSpecification {
  FramebufferTextureSpecification() = default;
  FramebufferTextureSpecification(FramebufferTextureFormat format)
      : texture_format(format) {}

  FramebufferTextureFormat texture_format = FramebufferTextureFormat::None;
  // TODO: filtering/wrap
};

struct FramebufferAttachmentSpecification {
  FramebufferAttachmentSpecification() = default;
  FramebufferAttachmentSpecification(
      std::initializer_list<FramebufferTextureSpecification> attachments)
      : attachments(attachments) {}

  std::vector<FramebufferTextureSpecification> attachments;
};



struct FramebufferSpecification {
  uint32_t width = 0, height = 0;
  FramebufferAttachmentSpecification attachments;
  uint32_t samples = 1;
  bool swap_chain_target = false;
};
class Framebuffer {
 public:
  Framebuffer() = default;
  virtual ~Framebuffer() = default;

  virtual void Bind() = 0;
  virtual void Unbind() = 0;
  virtual void Resize(uint32_t width, uint32_t height) = 0;
  virtual int ReadPixel(uint32_t attachment_index, int x, int y) = 0;
  virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
  virtual void ClearAttachment(uint32_t attachment_index, int value) = 0;
  virtual const FramebufferSpecification& GetSpecification() const = 0;
  static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
};
}
