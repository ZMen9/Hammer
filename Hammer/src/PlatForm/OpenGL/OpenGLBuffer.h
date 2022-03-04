#pragma once

#include "Hammer/Renderer/Buffer.h"

namespace hammer {

class OpenGLVertexBuffer : public VertexBuffer {
 public:
  OpenGLVertexBuffer(float* vertices, uint32_t size);
  virtual ~OpenGLVertexBuffer();

  virtual void Bind() const override;
  virtual void Unbind() const override;
  inline virtual void SetLayout(const BufferLayout& layout) override {
    layout_ = layout;
  }
  inline virtual const BufferLayout& GetLayout() const override{
    return layout_;
  }

 private:
  uint32_t render_id_;
  BufferLayout layout_;
};

class OpenGLIndexBuffer : public IndexBuffer {
 public:
  OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
  virtual ~OpenGLIndexBuffer();

  virtual void Bind() const override;
  virtual void Unbind() const override;
  inline virtual uint32_t GetCount() const override { return count_; }
 private:
  uint32_t render_id_;
  uint32_t count_;
};
}