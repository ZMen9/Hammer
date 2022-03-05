#include "hmpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>
namespace hammer {

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
  switch (type) {
    case hammer::ShaderDataType::Float:
      return GL_FLOAT;
    case hammer::ShaderDataType::Float2:
      return GL_FLOAT;
    case hammer::ShaderDataType::Float3:
      return GL_FLOAT;
    case hammer::ShaderDataType::Float4:
      return GL_FLOAT;
    case hammer::ShaderDataType::Mat3:
      return GL_FLOAT;
    case hammer::ShaderDataType::Mat4:
      return GL_FLOAT;
    case hammer::ShaderDataType::Int:
      return GL_INT;
    case hammer::ShaderDataType::Int2:
      return GL_INT;
    case hammer::ShaderDataType::Int3:
      return GL_INT;
    case hammer::ShaderDataType::Int4:
      return GL_INT;
    case hammer::ShaderDataType::Bool:
      return GL_BOOL;
  }
  HM_CORE_ASSERT(false, "Unknown ShaderDataType!");
  return 0;
}

OpenGLVertexArray::OpenGLVertexArray() {
  glGenVertexArrays(1, &render_id_);
}

OpenGLVertexArray::~OpenGLVertexArray() { glDeleteVertexArrays(1,&render_id_); }

void OpenGLVertexArray::Bind() const { glBindVertexArray(render_id_); }

void OpenGLVertexArray::Unbind() const { glBindVertexArray(0); }

void OpenGLVertexArray::AddVertexBuffer(
    const std::shared_ptr<VertexBuffer>& vertex_buffer) {
  HM_CORE_ASSERT(vertex_buffer->GetLayout().elements().size(),
                 "Vertex Buffer has no layout!");
  glBindVertexArray(render_id_);
  vertex_buffer->Bind();

  uint32_t index = 0;
  const auto& layout = vertex_buffer->GetLayout();
  for (const auto& element : layout) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, element.GetComponentCount(),
                          ShaderDataTypeToOpenGLBaseType(element.type_),
                          element.normalized_ ? GL_TRUE : GL_FALSE,
                          layout.stride(), (const void*)element.offset_);
    index++;
  }
  vertex_buffers_.push_back(vertex_buffer);
}

void OpenGLVertexArray::SetIndexBuffer(
    const std::shared_ptr<IndexBuffer>& index_buffer) {
  glBindVertexArray(render_id_);
  index_buffer->Bind();

  index_buffer_ = index_buffer;
}

} // namespace hammer