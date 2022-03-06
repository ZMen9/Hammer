#include "hmpch.h"
#include "Application.h"
#include "Hammer/Renderer/Renderer.h"
#include "Input.h"

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
namespace hammer {

Application* Application::instance_ = nullptr;

Application::Application() : camera_(-1.6f, 1.6f, -0.9f, 0.9f) {
  HM_CORE_ASSERT(!instance_, "Application already exists!");
  instance_ = this;
  window_ = std::unique_ptr<Window>(Window::Create());
  window_->SetEventCallback(BIND_EVENT_FN(OnEvent));

  ImGuiLayer_ = new ImGuiLayer();
  PushOverlay(ImGuiLayer_);

  float vertices[3 * 7] = {-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
                           0.5f,  -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
                           0.0f,  0.5f,  0.0f, 0.8f, 0.8f, 0.2f, 1.0f};

  vertex_array_.reset(VertexArray::Create());


  std::shared_ptr<VertexBuffer> vertex_buffer(
      VertexBuffer::Create(vertices, sizeof(vertices)));

  vertex_buffer->SetLayout({{ShaderDataType::Float3, "a_position"},
                            {ShaderDataType::Float4, "a_color"}});
  vertex_array_->AddVertexBuffer(vertex_buffer);

  unsigned int indices[3] = {0, 1, 2};
  std::shared_ptr<IndexBuffer> index_buffer(
      IndexBuffer::Creaet(indices, sizeof(indices) / sizeof(uint32_t)));
  vertex_array_->SetIndexBuffer(index_buffer);

  float squareVertices[3 * 4] = {
    -0.75f, -0.75f, 0.0f, 
     0.75f, -0.75f, 0.0f,
     0.75f,  0.75f, 0.0f, 
    -0.75f,  0.75f, 0.0f
  };

  square_va_.reset(VertexArray::Create());
  std::shared_ptr<VertexBuffer> square_vb(
      VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

  square_vb->SetLayout({{ShaderDataType::Float3, "a_position"}});
  square_va_->AddVertexBuffer(square_vb);

  unsigned int square_indices[6] = {0, 1, 2, 2, 3, 0};
  std::shared_ptr<IndexBuffer> square_ib(IndexBuffer::Creaet(
      square_indices, sizeof(square_indices) / sizeof(uint32_t)));
  square_va_->SetIndexBuffer(square_ib);


  std::string vertex_src = R"(
			#version 450 core
			layout(location = 0) in vec3 a_position;
      layout(location = 1) in vec4 a_color;

      uniform mat4 u_ViewProjection;
      
			out vec3 v_position;
      out vec4 v_color;
      
			void main()
			{
				v_position = a_position;
        v_color = a_color;
				gl_Position = u_ViewProjection * vec4(a_position, 1.0);	
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

  std::string blueShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			out vec3 v_Position;
      uniform mat4 u_ViewProjection;
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
			}
		)";

  std::string blueShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

  blue_shader_.reset(new Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
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
    RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    RenderCommand::Clear();

    camera_.set_position({0.0f, 0.0f, 0.0f});
    camera_.set_degrees(0.0f);


    Renderer::BeginScene(camera_);
    
    Renderer::Submit(blue_shader_ ,square_va_);
    Renderer::Submit(shader_, vertex_array_);

    Renderer::EndScene();
    
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
}

void Application::PushOverlay(Layer* overlay) {
  layer_stack_.PushOverlay(overlay);
}

}  // namespace hammer
