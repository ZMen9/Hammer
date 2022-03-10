#pragma once

#include "Hammer/Renderer/VertexArray.h"

namespace hammer {
class OpenGLVertexArray : public VertexArray {
 public:
  OpenGLVertexArray();
  virtual ~OpenGLVertexArray();
  virtual void Bind() const override;
  virtual void Unbind() const override;
  virtual void AddVertexBuffer(
      const Ref<VertexBuffer>& vertex_buffer) override;
  virtual void SetIndexBuffer(
      const Ref<IndexBuffer>& index_buffer) override;

  inline virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers()
      const override {
    return vertex_buffers_;
  }
  inline virtual const Ref<IndexBuffer>& GetIndexBuffer() const override {
    return index_buffer_;
  }

 private:
  uint32_t render_id_;
  uint32_t vertex_buffer_index_ = 0;
  std::vector<Ref<VertexBuffer>> vertex_buffers_;
  Ref<IndexBuffer> index_buffer_;
};
} // namespace hammer