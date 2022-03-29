#include "hmpch.h"
#include "PlatForm/OpenGL/OpenGLVertexArray.h"

#include <glad/glad.h>
namespace hammer {

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
  switch (type) {
    case ShaderDataType::Float:
      return GL_FLOAT;
    case ShaderDataType::Float2:
      return GL_FLOAT;
    case ShaderDataType::Float3:
      return GL_FLOAT;
    case ShaderDataType::Float4:
      return GL_FLOAT;
    case ShaderDataType::Mat3:
      return GL_FLOAT;
    case ShaderDataType::Mat4:
      return GL_FLOAT;
    case ShaderDataType::Int:
      return GL_INT;
    case ShaderDataType::Int2:
      return GL_INT;
    case ShaderDataType::Int3:
      return GL_INT;
    case ShaderDataType::Int4:
      return GL_INT;
    case ShaderDataType::Bool:
      return GL_BOOL;
  }
  HM_CORE_ASSERT(false, "Unknown ShaderDataType!");
  return 0;
}

OpenGLVertexArray::OpenGLVertexArray() {
  HM_PROFILE_FUNCTION();
  glGenVertexArrays(1, &render_id_);
}

OpenGLVertexArray::~OpenGLVertexArray() {
  HM_PROFILE_FUNCTION();
  glDeleteVertexArrays(1, &render_id_);
}

void OpenGLVertexArray::Bind() const {
  HM_PROFILE_FUNCTION();
  glBindVertexArray(render_id_);
}

void OpenGLVertexArray::Unbind() const {
  HM_PROFILE_FUNCTION();
  glBindVertexArray(0);
}

void OpenGLVertexArray::AddVertexBuffer(
    const Ref<VertexBuffer>& vertex_buffer) {
  HM_PROFILE_FUNCTION();
  HM_CORE_ASSERT(vertex_buffer->GetLayout().elements().size(),
                 "Vertex Buffer has no layout!");
  glBindVertexArray(render_id_);
  vertex_buffer->Bind();

  const auto& layout = vertex_buffer->GetLayout();
  for (const auto& element : layout) {
    switch (element.type_) {
      case ShaderDataType::Float:
      case ShaderDataType::Float2:
      case ShaderDataType::Float3:
      case ShaderDataType::Float4:
      case ShaderDataType::Int:
      case ShaderDataType::Int2:
      case ShaderDataType::Int3:
      case ShaderDataType::Int4:
      case ShaderDataType::Bool: {
        glEnableVertexAttribArray(vertex_buffer_index_);
        glVertexAttribPointer(vertex_buffer_index_, element.GetComponentCount(),
                              ShaderDataTypeToOpenGLBaseType(element.type_),
                              element.normalized_ ? GL_TRUE : GL_FALSE,
                              layout.stride(), (const void*)element.offset_);
        vertex_buffer_index_++;
        break;
      }
      case ShaderDataType::Mat3:
      case ShaderDataType::Mat4: {
        uint8_t count = element.GetComponentCount();
        for (uint8_t i = 0; i < count; i++) {
          glEnableVertexAttribArray(vertex_buffer_index_);
          glVertexAttribPointer(vertex_buffer_index_,
                                count,
                                ShaderDataTypeToOpenGLBaseType(element.type_),
                                element.normalized_ ? GL_TRUE : GL_FALSE,
                                layout.stride(), (const void*)element.offset_);
          glVertexAttribDivisor(vertex_buffer_index_, 1);
          vertex_buffer_index_++;
        }
        break;
      }
      default:
        HM_CORE_ASSERT(false, "Unknown ShaderDataType!");
    }
  }
  vertex_buffers_.push_back(vertex_buffer);
}

void OpenGLVertexArray::SetIndexBuffer(
    const Ref<IndexBuffer>& index_buffer) {
  HM_PROFILE_FUNCTION();
  glBindVertexArray(render_id_);
  index_buffer->Bind();

  index_buffer_ = index_buffer;
}

} // namespace hammer