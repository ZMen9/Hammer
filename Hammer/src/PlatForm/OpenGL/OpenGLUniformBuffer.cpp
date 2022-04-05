#include "hmpch.h"
#include "OpenGLUniformBuffer.h"
#include <glad/glad.h>
namespace hammer {

OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding_id) {
  glCreateBuffers(1, &renderer_id_);
  glNamedBufferData(renderer_id_, size, nullptr, GL_DYNAMIC_DRAW);// TODO: investigate usage hint
  glBindBufferBase(GL_UNIFORM_BUFFER, binding_id, renderer_id_);
}

OpenGLUniformBuffer::~OpenGLUniformBuffer() {
  glDeleteBuffers(1, &renderer_id_);
}

void OpenGLUniformBuffer::SetData(const void* data, uint32_t size,
                                   uint32_t offset /* = 0 */) {
  glNamedBufferSubData(renderer_id_, offset, size, data);
}

}  // namespace hammer