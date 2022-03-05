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
      const std::shared_ptr<VertexBuffer>& vertex_buffer) override;
  virtual void SetIndexBuffer(
      const std::shared_ptr<IndexBuffer>& index_buffer) override;

  inline virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers()
      const override {
    return vertex_buffers_;
  }
  inline virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override {
    return index_buffer_;
  }

 private:
  uint32_t render_id_;
  std::vector<std::shared_ptr<VertexBuffer>> vertex_buffers_;
  std::shared_ptr<IndexBuffer> index_buffer_;
};
} // namespace hammer