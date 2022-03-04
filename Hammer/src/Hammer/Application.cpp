#include "hmpch.h"
#include "Application.h"

#include <glad/glad.h>
#include "Input.h"

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
namespace hammer {

Application* Application::instance_ = nullptr;

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


Application::Application() {
  HM_CORE_ASSERT(!instance_, "Application already exists!");
  instance_ = this;
  window_ = std::unique_ptr<Window>(Window::Create());
  window_->SetEventCallback(BIND_EVENT_FN(OnEvent));

  ImGuiLayer_ = new ImGuiLayer();
  PushOverlay(ImGuiLayer_);

  glGenVertexArrays(1, &vertex_array_);
  glBindVertexArray(vertex_array_);
  

  float vertices[3 * 7] = {-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
                           0.5f,  -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
                           0.0f,  0.5f,  0.0f, 0.8f, 0.8f, 0.2f, 1.0f};


  vertex_buffer_.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

  {
    BufferLayout layout = {
      {ShaderDataType::Float3, "a_position"},
      {ShaderDataType::Float4, "a_color"}
    };

    vertex_buffer_->SetLayout(layout);
  }

  unsigned int index = 0;
  const auto& layout = vertex_buffer_->GetLayout();
  for (const auto& element : layout) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, element.GetComponentCount(),
                          ShaderDataTypeToOpenGLBaseType(element.type_),
                          element.normalized_ ? GL_TRUE : GL_FALSE,
                          layout.stride(),
                          (const void*)element.offset_);
    index++;
  }

  unsigned int indices[3] = {0, 1, 2};
  index_buffer_.reset(
      IndexBuffer::Creaet(indices, sizeof(indices) / sizeof(uint32_t)));
  

  std::string vertex_src = R"(
			#version 450 core
			layout(location = 0) in vec3 a_position;
      layout(location = 1) in vec4 a_color;
      
			out vec3 v_position;
      out vec4 v_color;
      
			void main()
			{
				v_position = a_position;
        v_color = a_color;
				gl_Position = vec4(a_position, 1.0);	
			}
		)";

  std::string fragment_src = R"(
			#version 450 core
			layout(location = 0) out vec4 color;
			in vec3 v_position;
      in vec4 v_color;
      
			void main()
			{
				color = vec4(v_position * 0.5 + 0.5, 1.0);
        color = v_color;
        
			}
		)";
  shader_.reset(new Shader(vertex_src, fragment_src));
}


void Application::OnEvent(Event& e) {
  EventDispatcher dispatcher(e);
  dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

  for (auto it = layer_stack_.end(); it != layer_stack_.begin();) {
    (*--it)->OnEvent(e);
    if (e.handled_) break;
  }
}

void Application::Run() {
  while (running_) {
    glClearColor(0.1f, 0.1f, 0.1f, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    shader_->Bind();
    glBindVertexArray(vertex_array_);
    glDrawElements(GL_TRIANGLES, index_buffer_->GetCount(), GL_UNSIGNED_INT, nullptr);
    

    for (Layer* layer : layer_stack_) {
      layer->OnUpdate();
    }

    ImGuiLayer_->Begin();
    for (Layer* layer : layer_stack_) {
      layer->OnImGuiRender();
    }
    ImGuiLayer_->End();
    window_->OnUpdate();
  }
}

bool Application::OnWindowClose(WindowCloseEvent& e) {
  running_ = false;
  return true;
}

void Application::PushLayer(Layer* layer) { 
  layer_stack_.PushLayer(layer);
  layer->OnAttach();
}

void Application::PushOverlay(Layer* overlay) {
  layer_stack_.PushOverlay(overlay);
  overlay->OnAttach();
}

}  // namespace hammer
