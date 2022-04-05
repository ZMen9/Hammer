#pragma once
#include "Hammer/Renderer/UniformBuffer.h"

namespace hammer {
class OpenGLUniformBuffer : public UniformBuffer {
 public:
  OpenGLUniformBuffer(uint32_t size, uint32_t binding_id);
  virtual ~OpenGLUniformBuffer();
  virtual void SetData(const void* data, uint32_t size,
                       uint32_t offset /* = 0 */) override;

 private:
  uint32_t renderer_id_ = 0;
};
}