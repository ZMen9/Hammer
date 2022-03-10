#include <Hammer.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "PlatForm/OpenGL/OpenGLShader.h"

class ExampleLayer : public hammer::Layer {
 public:
  ExampleLayer() : Layer("Example"), camera_(-1.6f, 1.6f, -0.9f, 0.9f), camera_position_({0.0f}) {
    float vertices[3 * 7] = {
      -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
       0.5f,  -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
       0.0f,  0.5f,  0.0f, 0.8f, 0.8f, 0.2f, 1.0f
    };

    vertex_array_.reset(hammer::VertexArray::Create());

    hammer::Ref<hammer::VertexBuffer> vertex_buffer(
        hammer::VertexBuffer::Create(vertices, sizeof(vertices)));

    vertex_buffer->SetLayout({{hammer::ShaderDataType::Float3, "a_position"},
                              {hammer::ShaderDataType::Float4, "a_color"}});
    vertex_array_->AddVertexBuffer(vertex_buffer);

    unsigned int indices[3] = {0, 1, 2};
    hammer::Ref<hammer::IndexBuffer> index_buffer(
        hammer::IndexBuffer::Creaet(indices,
                                    sizeof(indices) / sizeof(uint32_t)));
    vertex_array_->SetIndexBuffer(index_buffer);

    float squareVertices[5 * 4] = {
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
       0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
       0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
      -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
    };

    square_va_.reset(hammer::VertexArray::Create());
    hammer::Ref<hammer::VertexBuffer> square_vb(
        hammer::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

    square_vb->SetLayout({
      {hammer::ShaderDataType::Float3, "a_position"}, 
      {hammer::ShaderDataType::Float2, "a_texture_coord"}
      });
    square_va_->AddVertexBuffer(square_vb);

    unsigned int square_indices[6] = {0, 1, 2, 2, 3, 0};
    hammer::Ref<hammer::IndexBuffer> square_ib(hammer::IndexBuffer::Creaet(
        square_indices, sizeof(square_indices) / sizeof(uint32_t)));
    square_va_->SetIndexBuffer(square_ib);

    std::string vertex_src = R"(
			#version 450 core
			layout(location = 0) in vec3 a_position;
      layout(location = 1) in vec4 a_color;

      uniform mat4 u_ViewProjection;
      uniform mat4 u_Transform;
      
			out vec3 v_position;
      out vec4 v_color;
      
			void main()
			{
				v_position = a_position;
        v_color = a_color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_position, 1.0);	
			}
		)";

    std::string fragment_src = R"(
			#version 450 core
			layout(location = 0) out vec4 color;
			in vec3 v_position;
      in vec4 v_color;
      
			void main()
			{
        color = v_color;
        
			}
		)";
    shader_.reset(hammer::Shader::Create(vertex_src, fragment_src));

    std::string flatColorShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			out vec3 v_Position;
      uniform mat4 u_ViewProjection;
      uniform mat4 u_Transform;
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

    std::string flatColorShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
      uniform vec3 u_Color;
			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

    flat_color_shader_.reset(hammer::Shader::Create(flatColorShaderVertexSrc, flatColorShaderFragmentSrc));

    std::string textureShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
      layout(location = 1) in vec2 a_TextCoord;
			out vec3 v_Position;
      uniform mat4 u_ViewProjection;
      uniform mat4 u_Transform;
      out vec2 v_textcoord;

			void main()
			{
				v_textcoord = a_TextCoord;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

    std::string textureShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

      in vec2 v_textcoord;
      uniform sampler2D u_Texture;
			void main()
			{
				color = texture(u_Texture ,v_textcoord);
			}
		)";

    texture_shader_.reset(hammer::Shader::Create(textureShaderVertexSrc,
                                                    textureShaderFragmentSrc));

    texture_ = hammer::Texture2D::Create("assets/textures/Checkerboard.png");
    cherno_logo_texture_ =
        hammer::Texture2D::Create("assets/textures/ChernoLogo.png");

    texture_shader_->Bind();
    std::dynamic_pointer_cast<hammer::OpenGLShader>(texture_shader_)
        ->UploadUniformInt("u_Texture", 0);
  }

  void OnUpdate(hammer::Timestep ts) override {
    //HM_INFO("Delta Time: {0}s ({1}ms)", ts.GetSeconds(), ts.GetMilliseconds());
    if (hammer::Input::IsKeyPressed(HM_KEY_LEFT)) {
      camera_position_.x -= camera_move_speed_ * ts;
    }
    else if (hammer::Input::IsKeyPressed(HM_KEY_RIGHT)) {
      camera_position_.x += camera_move_speed_ * ts;
    }
    if (hammer::Input::IsKeyPressed(HM_KEY_UP)) {
      camera_position_.y += camera_move_speed_ * ts;
    }
    else if (hammer::Input::IsKeyPressed(HM_KEY_DOWN)) {
      camera_position_.y -= camera_move_speed_ * ts;
    }
    if (hammer::Input::IsKeyPressed(HM_KEY_A)) {
      camera_degrees_ -= camera_rotation_speed_ * ts;
    }
    if (hammer::Input::IsKeyPressed(HM_KEY_D)) {
      camera_degrees_ += camera_rotation_speed_ * ts;
    }

    hammer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    hammer::RenderCommand::Clear();

    camera_.set_position(camera_position_);
    camera_.set_degrees(camera_degrees_);

    hammer::Renderer::BeginScene(camera_);

    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

    flat_color_shader_->Bind();
    std::dynamic_pointer_cast<hammer::OpenGLShader>(flat_color_shader_)
        ->UploadUniformFloat3("u_Color", square_color_);

    for (int y = 0; y < 20; ++y) {
      for (int x = 0; x < 20; ++x) {
        glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
        hammer::Renderer::Submit(flat_color_shader_, square_va_, transform);
      }
    }

    texture_->Bind();
    hammer::Renderer::Submit(texture_shader_, square_va_, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
    cherno_logo_texture_->Bind();
    hammer::Renderer::Submit(texture_shader_, square_va_,
                             glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));


    // hammer::Renderer::Submit(shader_, vertex_array_);

    hammer::Renderer::EndScene();
  }

  void OnEvent(hammer::Event& event) override {
  }

  void OnImGuiRender() override { 
    ImGui::Begin("Settings");
    ImGui::ColorEdit3("Square Color", glm::value_ptr(square_color_));
    ImGui::End();
  }

 private:
  // will use our memory managerment here instead of use hammer::Ref
  hammer::Ref<hammer::Shader> shader_;
  hammer::Ref<hammer::VertexArray> vertex_array_;
  hammer::Ref<hammer::Shader> flat_color_shader_ , texture_shader_;
  hammer::Ref<hammer::VertexArray> square_va_;

  hammer::Ref<hammer::Texture2D> texture_ , cherno_logo_texture_;

  hammer::OrthographicCamera camera_;
  glm::vec3 camera_position_;
  float camera_degrees_ = 0.0f;
  float camera_move_speed_ = 5.0f ;
  float camera_rotation_speed_ = 180.0f;

  glm::vec3 square_color_ = {0.2f, 0.3f, 0.8f};
};

class Sandbox : public hammer::Application {
 public:
  Sandbox() { PushLayer(new ExampleLayer()); }
  ~Sandbox(){};
};

hammer::Application* hammer::CreateApplication() { return new Sandbox(); }