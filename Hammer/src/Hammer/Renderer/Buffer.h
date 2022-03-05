#pragma once

namespace hammer {

enum class ShaderDataType {
  None = 0,
  Float,
  Float2,
  Float3,
  Float4,
  Mat3,
  Mat4,
  Int,
  Int2,
  Int3, 
  Int4,
  Bool
};

static uint32_t ShaderDataTypeSize(ShaderDataType type) { 
  switch (type) { 
    case ShaderDataType::Float:
      return 4;
    case ShaderDataType::Float2:
      return 4 * 2;
    case ShaderDataType::Float3:
      return 4 * 3;
    case ShaderDataType::Float4:
      return 4 * 4;
    case ShaderDataType::Mat3:
      return 4 * 3 * 3;
    case ShaderDataType::Mat4:
      return 4 * 4 * 4;
    case ShaderDataType::Int:
      return 4;
    case ShaderDataType::Int2:
      return 4 * 2;
    case ShaderDataType::Int3:
      return 4 * 3;
    case ShaderDataType::Int4:
      return 4 * 4;
    case ShaderDataType::Bool:
      return 1;
  }

  HM_CORE_ASSERT(false, "Unknown ShaderDataType!");
  return 0;
}

struct BufferElement {
  std::string name_;
  ShaderDataType type_;
  uint32_t size_ = 0;
  uint32_t offset_ = 0;
  bool normalized_ = false;
  BufferElement() = default;

  BufferElement(ShaderDataType type, const std::string& name,
    bool normalized = false)
      : name_(name),
        type_(type),
        size_(ShaderDataTypeSize(type)),
        offset_(0),
        normalized_(normalized) {}

  uint32_t GetComponentCount() const {
    switch (type_) {
      case ShaderDataType::Float:
        return 1;
      case ShaderDataType::Float2:
        return 2;
      case ShaderDataType::Float3:
        return 3;
      case ShaderDataType::Float4:
        return 4;
      case ShaderDataType::Mat3:
        return 3 * 3;
      case ShaderDataType::Mat4:
        return 4 * 4;
      case ShaderDataType::Int:
        return 1;
      case ShaderDataType::Int2:
        return 2;
      case ShaderDataType::Int3:
        return 3;
      case ShaderDataType::Int4:
        return 4;
      case ShaderDataType::Bool:
        return 1;
    }

    HM_CORE_ASSERT(false, "Unknown ShaderDataType!");
    return 0;
  }

};

class BufferLayout {
 public:
  BufferLayout() = default;

  BufferLayout(const std::initializer_list<BufferElement>& elements)
      : elements_(elements) {
    CalculateOffsetsAndStride();
  }

  inline uint32_t stride() const { return stride_; }
  inline const std::vector<BufferElement>& elements() const { return elements_; }

  std::vector<BufferElement>::iterator begin() { return elements_.begin(); }
  std::vector<BufferElement>::iterator end() { return elements_.end(); }
  std::vector<BufferElement>::const_iterator begin() const{ return elements_.begin(); }
  std::vector<BufferElement>::const_iterator end() const { return elements_.end(); }

 private:
  void CalculateOffsetsAndStride() { 
    stride_ = 0;
    for (auto& element : elements_) {
      element.offset_ = stride_;
      stride_ += element.size_;
    }
  }
 private:
  std::vector<BufferElement> elements_;
  uint32_t stride_ = 0;
};

class VertexBuffer {
 public:
  virtual ~VertexBuffer() = default;

  virtual void Bind() const = 0;
  virtual void Unbind() const = 0;
  virtual void SetLayout(const BufferLayout& layout) = 0;
  virtual const BufferLayout& GetLayout() const = 0;

  static VertexBuffer* Create(float* vertices, uint32_t size);
};

class IndexBuffer {
 public:
  virtual ~IndexBuffer() = default;

  virtual void Bind() const = 0;
  virtual void Unbind() const = 0;
  virtual uint32_t GetCount() const = 0;

  static IndexBuffer* Creaet(uint32_t* indices, uint32_t size);
};
}  // namespace hammer