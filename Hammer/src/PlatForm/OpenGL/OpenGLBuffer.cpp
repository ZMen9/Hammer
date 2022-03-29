#include "hmpch.h"
#include "PlatForm/OpenGL/OpenGLBuffer.h"
#include <glad/glad.h>


namespace hammer {
//////////////////////////////////////////////////////////////////////////
// OpenGLVertexBuffer ////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size) { 
  HM_PROFILE_FUNCTION();
  glCreateBuffers(1, &render_id_);
  glBindBuffer(GL_ARRAY_BUFFER, render_id_);
  glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size) {
  HM_PROFILE_FUNCTION();
  glCreateBuffers(1, &render_id_);
  glBindBuffer(GL_ARRAY_BUFFER, render_id_);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
  HM_PROFILE_FUNCTION();
  glDeleteBuffers(1, &render_id_);
}

void OpenGLVertexBuffer::Bind() const {
  HM_PROFILE_FUNCTION();
  glBindBuffer(GL_ARRAY_BUFFER, render_id_);
}

void OpenGLVertexBuffer::Unbind() const {
  HM_PROFILE_FUNCTION();
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void OpenGLVertexBuffer::SetData(const void* data, uint32_t size) {
  glBindBuffer(GL_ARRAY_BUFFER, render_id_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

//////////////////////////////////////////////////////////////////////////
// OpenGLIndexBuffer /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) 
    : count_(count) { 
  HM_PROFILE_FUNCTION();
  glCreateBuffers(1, &render_id_);

  // GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
  // Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO
  // state. 
  glBindBuffer(GL_ARRAY_BUFFER, render_id_);
  glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices,
               GL_STATIC_DRAW);
  
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() {
  HM_PROFILE_FUNCTION();
  glDeleteBuffers(1, &render_id_);
}

void OpenGLIndexBuffer::Bind() const {
  HM_PROFILE_FUNCTION();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_id_);
}

void OpenGLIndexBuffer::Unbind() const {
  HM_PROFILE_FUNCTION();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}  // namespace hammer