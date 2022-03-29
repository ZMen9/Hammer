#include "ExampleLayer.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

ExampleLayer::ExampleLayer()
    : Layer("Example"), camera_controller_(1920.0f / 1080.0f) {
  float vertices[3 * 7] = {-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
                           0.5f,  -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
                           0.0f,  0.5f,  0.0f, 0.8f, 0.8f, 0.2f, 1.0f};

  vertex_array_ = hammer::VertexArray::Create();

  hammer::Ref<hammer::VertexBuffer> vertex_buffer =
      hammer::VertexBuffer::Create(vertices, sizeof(vertices));

  vertex_buffer->SetLayout({{hammer::ShaderDataType::Float3, "a_position"},
                            {hammer::ShaderDataType::Float4, "a_color"}});
  vertex_array_->AddVertexBuffer(vertex_buffer);

  unsigned int indices[3] = {0, 1, 2};
  hammer::Ref<hammer::IndexBuffer> index_buffer =
      hammer::IndexBuffer::Creaet(indices, sizeof(indices) / sizeof(uint32_t));
  vertex_array_->SetIndexBuffer(index_buffer);

  float squareVertices[5 * 4] = {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.5f, -0.5f,
                                 0.0f,  1.0f,  0.0f, 0.5f, 0.5f, 0.0f, 1.0f,
                                 1.0f,  -0.5f, 0.5f, 0.0f, 0.0f, 1.0f};

  square_va_ = hammer::VertexArray::Create();
  hammer::Ref<hammer::VertexBuffer> square_vb(
      hammer::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

  square_vb->SetLayout({{hammer::ShaderDataType::Float3, "a_position"},
                        {hammer::ShaderDataType::Float2, "a_texture_coord"}});
  square_va_->AddVertexBuffer(square_vb);

  unsigned int square_indices[6] = {0, 1, 2, 2, 3, 0};
  hammer::Ref<hammer::IndexBuffer> square_ib(hammer::IndexBuffer::Creaet(
      square_indices, sizeof(square_indices) / sizeof(uint32_t)));
  square_va_->SetIndexBuffer(square_ib);

  std::string vertex_src = R"(
                        #version 330 core
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
                                gl_Position = u_ViewProjection * u_Transform *
    vec4(a_position, 1.0);
                        }
                )";

  std::string fragment_src = R"(
                        #version 330 core
                        layout(location = 0) out vec4 color;
                        in vec3 v_position;
      in vec4 v_color;

                        void main()
                        {
        color = v_color;

                        }
                )";
  shader_ =
      hammer::Shader::Create("vextex_pos_color", vertex_src, fragment_src);

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
    })";

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

  flat_color_shader_ = hammer::Shader::Create(
      "flat_color", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

  auto texture_shader = shader_library_.Load("assets/shaders/Texture.glsl");

  texture_ = hammer::Texture2D::Create("assets/textures/Checkerboard.png");
  cherno_logo_texture_ =
      hammer::Texture2D::Create("assets/textures/ChernoLogo.png");

  texture_shader->Bind();
  texture_shader->SetInt("u_Texture", 0);

  //// draw a sphere :
  // std::vector<float> sphere_vertices;
  // std::vector<unsigned int> sphere_indices;
  // const int kYSegment = 50;
  // const int kXSegment = 50;
  // const float kPI = 3.1415926535f;

  // for (int y = 0; y <= kYSegment; ++y) {
  //  for (int x = 0; x <= kXSegment; ++x) {
  //    float x_segment = (float)x / (float)kXSegment;
  //    float y_segment = (float)y / (float)kYSegment;
  //    float x_pos = std::cos(x_segment * 2.0f * kPI) * std::sin(y_segment *
  // kPI); float y_pos = std::cos(y_segment * kPI); float z_pos =
  //        std::sin(x_segment * 2.0f * kPI) * std::cos(y_segment * kPI);
  //    sphere_vertices.push_back(x_pos);
  //    sphere_vertices.push_back(y_pos);
  //    sphere_vertices.push_back(z_pos);
  //  }
  //}

  // for (int i = 0; i < kYSegment; ++i) {
  //  for (int j = 0; j < kXSegment; ++j) {
  //    sphere_indices.push_back(i * (kXSegment + 1) + j);
  //    sphere_indices.push_back((i + 1) * (kXSegment + 1) + j);
  //    sphere_indices.push_back((i + 1) * (kXSegment + 1) + j + 1);
  //    sphere_indices.push_back(i * (kXSegment + 1) + j);
  //    sphere_indices.push_back((i + 1) * (kXSegment + 1) + j + 1);
  //    sphere_indices.push_back(i * (kXSegment + 1) + j + 1);
  //  }
  //}

  // std::string sphere_vertex_src = R"(
  //  #version 330 core

  //  layout(location = 0) in vec3 a_Position;
  //  uniform mat4 u_ViewProjection;
  //  uniform mat4 u_Transform;
  //  void main()
  //  {
  //    gl_Position = u_ViewProjection * u_Transform *vec4(a_Position, 1.0);
  //  })";

  // std::string sphere_fragment_src = R"(
  //   #version 330 core
  //   layout(location = 0) out vec4 color;
  //   uniform vec3 u_Color;
  //   void main()
  //   {
  //     color = vec4(u_Color, 1.0);
  //   })";

  // sphere_va_.reset(hammer::VertexArray::Create());
  // hammer::Ref<hammer::VertexBuffer> sphere_vb(
  //    hammer::VertexBuffer::Create(&sphere_vertices[0],
  //(uint32_t)sphere_vertices.size() * sizeof(float)));

  // sphere_vb->SetLayout({{hammer::ShaderDataType::Float3, "a_position"}});
  // sphere_va_->AddVertexBuffer(sphere_vb);

  // hammer::Ref<hammer::IndexBuffer> sphere_ib(hammer::IndexBuffer::Creaet(
  //    &sphere_indices[0], (uint32_t)sphere_indices.size()));
  // sphere_va_->SetIndexBuffer(sphere_ib);
  // sphere_shader_ = hammer::Shader::Create("sphere", sphere_vertex_src,
  // sphere_fragment_src);
  ////
}

void ExampleLayer::OnAttach() {}

void ExampleLayer::OnDetach() {}

void ExampleLayer::OnUpdate(hammer::Timestep ts) {
  hammer::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
  hammer::RenderCommand::Clear();

  camera_controller_.OnUpdate(ts);

  hammer::Renderer::BeginScene(camera_controller_.GetCamera());

  /*sphere_shader_->Bind();
  std::dynamic_pointer_cast<hammer::OpenGLShader>(sphere_shader_)
      ->UploadUniformFloat3("u_Color", square_color_);
  hammer::Renderer::Submit(sphere_shader_, sphere_va_,
                           glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)));*/

  glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

  flat_color_shader_->Bind();
  flat_color_shader_->SetFloat3("u_Color", square_color_);

  for (int y = 0; y < 20; ++y) {
    for (int x = 0; x < 20; ++x) {
      glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
      glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
      hammer::Renderer::Submit(flat_color_shader_, square_va_, transform);
    }
  }

  auto texture_shader = shader_library_.Get("Texture");

  texture_->Bind();
  hammer::Renderer::Submit(texture_shader, square_va_,
                           glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

  cherno_logo_texture_->Bind();
  hammer::Renderer::Submit(texture_shader, square_va_,
                           glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

  // hammer::Renderer::Submit(shader_, vertex_array_);

  hammer::Renderer::EndScene();
}

void ExampleLayer::OnEvent(hammer::Event& event) {
  camera_controller_.OnEvent(event);
}

void ExampleLayer::OnImGuiRender() {
  ImGui::Begin("Settings");
  ImGui::ColorEdit3("Square Color", glm::value_ptr(square_color_));
  ImGui::End();
}