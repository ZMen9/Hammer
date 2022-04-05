#include "hmpch.h"
#include "Hammer/Renderer/Renderer2D.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Hammer/Renderer/VertexArray.h"
#include "Hammer/Renderer/Shader.h"
#include "Hammer/Renderer/UniformBuffer.h"
#include "Hammer/Renderer/RenderCommand.h"
#include <glm/glm.hpp>

namespace hammer {
struct LineVertex {
  glm::vec3 position;
  glm::vec4 color;

  // Editor-only
  int entity_id;
};

struct QuadVertex {
  glm::vec3 position;
  glm::vec4 color;
  glm::vec2 tex_cood;
  float tex_idx;
  float tiling_factor;
  // Editor only
  int entity_id;

};

struct CircleVertex {
  glm::vec3 world_position;
  glm::vec3 local_position;
  glm::vec4 color;
  float thickness;
  float fade;

  // Editor-only
  int entity_id;
};

struct Renderer2DData {
  static const uint32_t max_quads = 20000;
  static const uint32_t max_vertices = max_quads * 4;
  static const uint32_t max_indices = max_quads * 6;
  static const uint32_t max_texture_slots = 32;  // TODO: render caps
  // Line
  Ref<VertexArray> line_vertex_array;
  Ref<VertexBuffer> line_vertex_buffer;
  Ref<Shader> line_shader;
  uint32_t line_index_count = 0;
  LineVertex* line_vertex_buffer_base = nullptr;
  LineVertex* line_vertex_buffer_ptr = nullptr;
  float line_width = 2.0f;

  // Quad
  Ref<VertexArray> quad_vertex_array;
  Ref<VertexBuffer> quad_vertex_buffer;
  Ref<Shader> quad_shader;
  Ref<Texture2D> white_texture;
  uint32_t quad_index_count = 0;
  QuadVertex* quad_vertex_buffer_base = nullptr;
  QuadVertex* quad_vertex_buffer_ptr = nullptr;
  // Circle
  Ref<VertexArray> circle_vertex_array;
  Ref<VertexBuffer> circle_vertex_buffer;
  Ref<Shader> circle_shader;
  uint32_t circle_index_count = 0;
  CircleVertex* circle_vertex_buffer_base = nullptr;
  CircleVertex* circle_vertex_buffer_ptr = nullptr;

  std::array<Ref<Texture2D>, max_texture_slots> texture_slots;
  uint32_t texture_slot_idx = 1;// 0 = white texture

  glm::vec4 quad_vertex_position[4];
  
  Renderer2D::Statistics stats;
  struct CameraData {
    glm::mat4 view_projection_matrix;
  };
  CameraData CameraBuffer;
  Ref<UniformBuffer> CameraUniformBuffer;
};

static Renderer2DData kData;

void Renderer2D::Init() {
  HM_PROFILE_FUNCTION();
  // Line
  kData.line_vertex_array = VertexArray::Create();
  kData.line_vertex_buffer =
      VertexBuffer::Create(kData.max_vertices * sizeof(LineVertex));
  kData.line_vertex_buffer->SetLayout({{ShaderDataType::Float3, "a_Position"},
                                       {ShaderDataType::Float4, "a_Color"},
                                       {ShaderDataType::Int, "a_EntityID"}});
  kData.line_vertex_array->AddVertexBuffer(kData.line_vertex_buffer);
  kData.line_vertex_buffer_base = new LineVertex[kData.max_vertices];

  // Quad
  kData.quad_vertex_array = VertexArray::Create();

  // vertex data copied to GPU
  kData.quad_vertex_buffer =
      VertexBuffer::Create(kData.max_vertices * sizeof(QuadVertex));
  kData.quad_vertex_buffer->SetLayout({
    {ShaderDataType::Float3, "a_Position"},
    {ShaderDataType::Float4, "a_Color"},
    {ShaderDataType::Float2, "a_TextCoord"},
    {ShaderDataType::Float, "a_TextIdx"},
    {ShaderDataType::Float, "a_TilingFactor"},
    {ShaderDataType::Int,"a_EntityID"}
  });
  kData.quad_vertex_array->AddVertexBuffer(kData.quad_vertex_buffer);
  
  // vertex data from application on CPU
  kData.quad_vertex_buffer_base = new QuadVertex[kData.max_vertices];

  uint32_t* quad_indices = new uint32_t[kData.max_indices];
  uint32_t offset = 0;
  for (uint32_t i = 0; i < kData.max_indices; i += 6) {
    quad_indices[i + 0] = offset + 0;
    quad_indices[i + 1] = offset + 1;
    quad_indices[i + 2] = offset + 2;

    quad_indices[i + 3] = offset + 2;
    quad_indices[i + 4] = offset + 3;
    quad_indices[i + 5] = offset + 0;

    offset += 4;
  }
  Ref<IndexBuffer> quad_idx_buf =
      IndexBuffer::Creaet(quad_indices, kData.max_indices);
  kData.quad_vertex_array->SetIndexBuffer(quad_idx_buf);
  delete[] quad_indices;
  // end of quad
  
  // Circle
  kData.circle_vertex_array = VertexArray::Create();
  kData.circle_vertex_buffer =
      VertexBuffer::Create(kData.max_vertices * sizeof(CircleVertex));
  kData.circle_vertex_buffer->SetLayout(
      {{ShaderDataType::Float3, "a_WorldPosition"},
       {ShaderDataType::Float3, "a_LocalPosition"},
       {ShaderDataType::Float4, "a_Color"},
       {ShaderDataType::Float, "a_Thickness"},
       {ShaderDataType::Float, "a_Fade"},
       {ShaderDataType::Int, "a_EntityID"}});
  kData.circle_vertex_array->AddVertexBuffer(kData.circle_vertex_buffer);
  kData.circle_vertex_array->SetIndexBuffer(quad_idx_buf);// use quad IB
  kData.circle_vertex_buffer_base = new CircleVertex[kData.max_vertices];
  // end of circle
  
  // texture and texture shader
  kData.white_texture = Texture2D::Create(1, 1);
  uint32_t white_texturedata = 0xffffffff;
  kData.white_texture->SetData(&white_texturedata, sizeof(uint32_t));

  //int32_t samplers[kData.max_texture_slots];
  // for (uint32_t i = 0; i < kData.max_texture_slots; i++) {
   // samplers[i] = i;
  //}
  // 设置纹理着色器
  kData.quad_shader = Shader::Create("assets/shaders/Renderer2D_Quad.glsl");
  kData.circle_shader = Shader::Create("assets/shaders/Renderer2D_Circle.glsl");
  kData.line_shader = Shader::Create("assets/shaders/Renderer2D_Line.glsl");
  // wont use these, we use uniform buffer:
  //kData.quad_shader->Bind();
  //kData.quad_shader->SetIntArray("u_Textures", samplers,
                                    //kData.max_texture_slots);
  // set first texture slot to 0
  kData.texture_slots[0] = kData.white_texture;

  // 设置矩形四个顶点初始位置
  kData.quad_vertex_position[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
  kData.quad_vertex_position[1] = { 0.5f, -0.5f, 0.0f, 1.0f};
  kData.quad_vertex_position[2] = { 0.5f,  0.5f, 0.0f, 1.0f};
  kData.quad_vertex_position[3] = {-0.5f,  0.5f, 0.0f, 1.0f};
  // Init the UniformBuffer
  kData.CameraUniformBuffer =
      UniformBuffer::Create(sizeof(Renderer2DData::CameraBuffer), 0);
}

void Renderer2D::Shutdown() {
  HM_PROFILE_FUNCTION();

  delete[] kData.line_vertex_buffer_base;
  delete[] kData.quad_vertex_buffer_base;
  delete[] kData.circle_vertex_buffer_base;
}

void Renderer2D::BeginScene(const OrthographicCamera& camera) {
  HM_PROFILE_FUNCTION();
  kData.CameraBuffer.view_projection_matrix = camera.view_projection_matrix();
  kData.CameraUniformBuffer->SetData(&kData.CameraBuffer,
                                     sizeof(Renderer2DData::CameraData));
  StartBatch();
}

void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform) {
  HM_PROFILE_FUNCTION();
  kData.CameraBuffer.view_projection_matrix =
      camera.projection() * glm::inverse(transform);
  kData.CameraUniformBuffer->SetData(&kData.CameraBuffer,
                                     sizeof(Renderer2DData::CameraBuffer));
  StartBatch();
}

void Renderer2D::BeginScene(const EditorCamera& camera) {
  HM_PROFILE_FUNCTION();
  kData.CameraBuffer.view_projection_matrix = camera.GetViewProjection();
  kData.CameraUniformBuffer->SetData(&kData.CameraBuffer,
                                     sizeof(Renderer2DData::CameraBuffer));
  StartBatch();
}

void Renderer2D::EndScene() { 
  HM_PROFILE_FUNCTION();
  Flush();
}

void Renderer2D::Flush() {
  //if (kData.quad_index_count == 0) return;// Nothing to draw  
  // quad
  if (kData.quad_index_count) {
    uint32_t data_size = (uint32_t)((uint8_t*)kData.quad_vertex_buffer_ptr -
                                    (uint8_t*)kData.quad_vertex_buffer_base);
    kData.quad_vertex_buffer->SetData(kData.quad_vertex_buffer_base, data_size);

    // Bind textures
    for (uint32_t i = 0; i < kData.texture_slot_idx; i++)
      kData.texture_slots[i]->Bind(i);
    kData.quad_shader->Bind();
    RenderCommand::DrawIndexed(kData.quad_vertex_array, kData.quad_index_count);
    kData.stats.draw_calls++;
  }
  // circle
  if (kData.circle_index_count) {
    uint32_t data_size = (uint32_t)((uint8_t*)kData.circle_vertex_buffer_ptr -
                                    (uint8_t*)kData.circle_vertex_buffer_base);
    kData.circle_vertex_buffer->SetData(kData.circle_vertex_buffer_base, data_size);
    kData.circle_shader->Bind();
    RenderCommand::DrawIndexed(kData.circle_vertex_array, kData.circle_index_count);
    kData.stats.draw_calls++;
  }
  // line
  if (kData.line_index_count) {
    uint32_t data_size = (uint32_t)((uint8_t*)kData.line_vertex_buffer_ptr -
                                    (uint8_t*)kData.line_vertex_buffer_base);
    kData.line_vertex_buffer->SetData(kData.line_vertex_buffer_base,
                                        data_size);
    kData.line_shader->Bind();
    RenderCommand::SetLineWidth(kData.line_width);
    RenderCommand::DrawLines(kData.line_vertex_array,
                               kData.line_index_count);
    kData.stats.draw_calls++;
  }

}


void Renderer2D::StartBatch() {
  // line
  kData.line_index_count = 0;
  kData.line_vertex_buffer_ptr = kData.line_vertex_buffer_base;
  // quad
  kData.quad_index_count = 0;
  kData.quad_vertex_buffer_ptr = kData.quad_vertex_buffer_base;
  // circle
  kData.circle_index_count = 0;
  kData.circle_vertex_buffer_ptr = kData.circle_vertex_buffer_base;

  kData.texture_slot_idx = 1;
}

void Renderer2D::NextBatch() {
  Flush();
  StartBatch();
}

void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entity_id) {
  HM_PROFILE_FUNCTION();
  if (kData.quad_index_count >= Renderer2DData::max_indices) NextBatch();

  const float texture_idx = 0.0f;  // default white texture
  constexpr size_t quad_vertex_count = 4;
  constexpr glm::vec2 texture_coords[] = {
      {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
  const float tiling_factor = 1.0f;

  for (size_t i = 0; i < quad_vertex_count; i++) {
    kData.quad_vertex_buffer_ptr->position =
        transform * kData.quad_vertex_position[i];
    kData.quad_vertex_buffer_ptr->color = color;
    kData.quad_vertex_buffer_ptr->tex_cood = texture_coords[i];
    kData.quad_vertex_buffer_ptr->tex_idx = texture_idx;
    kData.quad_vertex_buffer_ptr->tiling_factor = tiling_factor;
    kData.quad_vertex_buffer_ptr->entity_id = entity_id;
    kData.quad_vertex_buffer_ptr++;
  }
  kData.quad_index_count += 6;
  kData.stats.quad_count++;
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,
                          const glm::vec4& color) {
  DrawQuad({position.x, position.y, 0.0f}, size, color);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size,
                          const glm::vec4& color) {
  HM_PROFILE_FUNCTION();
  glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                        glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
  DrawQuad(transform, color);
}

void Renderer2D::DrawQuad(const glm::mat4& transform,
                          const Ref<Texture2D>& texture,
                          float tiling_factor,
                          const glm::vec4& tint_color, int entity_id) {
  HM_PROFILE_FUNCTION();
  if (kData.quad_index_count >= Renderer2DData::max_indices) NextBatch();

  constexpr size_t quad_vertex_count = 4;
  constexpr glm::vec2 texture_coords[] = {
      {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
  // 查询纹理是否存在
  float texture_idx = 0.0f;
  for (uint32_t i = 1; i < kData.texture_slot_idx; ++i) {
    if (*kData.texture_slots[i] == *texture) {
      texture_idx = (float)i;
      break;
    }
  }
  // 纹理不存在则添加纹理
  if (texture_idx == 0.0f) {
    if (kData.texture_slot_idx >= Renderer2DData::max_texture_slots)
      NextBatch();
    texture_idx = (float)kData.texture_slot_idx;
    kData.texture_slots[kData.texture_slot_idx] = texture;
    kData.texture_slot_idx++;
  }

  for (size_t i = 0; i < quad_vertex_count; i++) {
    kData.quad_vertex_buffer_ptr->position =
        transform * kData.quad_vertex_position[i];
    kData.quad_vertex_buffer_ptr->color = tint_color;
    kData.quad_vertex_buffer_ptr->tex_cood = texture_coords[i];
    kData.quad_vertex_buffer_ptr->tex_idx = texture_idx;
    kData.quad_vertex_buffer_ptr->tiling_factor = tiling_factor;
    kData.quad_vertex_buffer_ptr->entity_id = entity_id;
    kData.quad_vertex_buffer_ptr++;
  }
  kData.quad_index_count += 6;
  kData.stats.quad_count++;
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,
                          const Ref<Texture2D>& texture,
                          float tiling_factor /*= 1.0f*/,
                          const glm::vec4& tint_color /*= glm::vec4(1.0f)*/) {
  DrawQuad({position.x, position.y, 0.0f}, size, texture, tiling_factor,
           tint_color);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size,
                          const Ref<Texture2D>& texture, float tiling_factor,
                          const glm::vec4& tint_color) {
  HM_PROFILE_FUNCTION();
  glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                        glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
  DrawQuad(transform, texture, tiling_factor, tint_color);
}


void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,
                          const Ref<SubTexture2D>& subtexture,
                          float tiling_factor /*= 1.0f*/,
                          const glm::vec4& tint_color /*= glm::vec4(1.0f)*/) {
  DrawQuad({position.x, position.y, 0.0f}, size, subtexture, tiling_factor,
           tint_color);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size,
                          const Ref<SubTexture2D>& subtexture,
                          float tiling_factor /*= 1.0f*/,
                          const glm::vec4& tint_color /*= glm::vec4(1.0f)*/) {
  HM_PROFILE_FUNCTION();
  if (kData.quad_index_count >= Renderer2DData::max_indices) NextBatch();

  constexpr size_t quad_vertex_count = 4;
  const glm::vec2* texture_coords = subtexture->GetTexCoords();
  const Ref<Texture2D> texture = subtexture->GetTexture();

  // 查询纹理是否存在
  float texture_idx = 0.0f;
  for (uint32_t i = 1; i < kData.texture_slot_idx; ++i) {
    if (*kData.texture_slots[i] == *texture) {
      texture_idx = (float)i;
      break;
    }
  }
  // 纹理不存在则添加纹理
  if (texture_idx == 0.0f) {
    if (kData.texture_slot_idx >= Renderer2DData::max_texture_slots)
      NextBatch();
    texture_idx = (float)kData.texture_slot_idx;
    kData.texture_slots[kData.texture_slot_idx] = texture;
    kData.texture_slot_idx++;
  }

  glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                        glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

  for (size_t i = 0; i < quad_vertex_count; i++) {
    kData.quad_vertex_buffer_ptr->position =
        transform * kData.quad_vertex_position[i];
    kData.quad_vertex_buffer_ptr->color = tint_color;
    kData.quad_vertex_buffer_ptr->tex_cood = texture_coords[i];
    kData.quad_vertex_buffer_ptr->tex_idx = texture_idx;
    kData.quad_vertex_buffer_ptr->tiling_factor = tiling_factor;
    kData.quad_vertex_buffer_ptr++;
  }
  kData.quad_index_count += 6;
  kData.stats.quad_count++;
}

void Renderer2D::DrawRotatedQuad(const glm::vec2& position,
                                 const glm::vec2& size, float degree,
                                 const glm::vec4& color) {
  DrawRotatedQuad({position.x, position.y, 0.0f}, size, degree, color);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3& position,
                                 const glm::vec2& size, float degree,
                                 const glm::vec4& color) {
  HM_PROFILE_FUNCTION();

  glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), position) *
      glm::rotate(glm::mat4(1.0f), glm::radians(degree), {0.0f, 0.0f, 1.0f}) *
      glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

	DrawQuad(transform, color);
}

void Renderer2D::DrawRotatedQuad(
    const glm::vec2& position, const glm::vec2& size, float degree,
    const Ref<Texture2D>& texture, float tiling_factor /*= 1.0f*/,
    const glm::vec4& tint_color /*= glm::vec4(1.0f)*/) {
  DrawRotatedQuad({position.x, position.y, 0.0f}, size, degree, texture,
                  tiling_factor, tint_color);
}

void Renderer2D::DrawRotatedQuad(
    const glm::vec3& position, const glm::vec2& size, float degree,
    const Ref<Texture2D>& texture, float tiling_factor /*= 1.0f*/,
    const glm::vec4& tint_color /*= glm::vec4(1.0f)*/) {
  HM_PROFILE_FUNCTION();
  
  glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), position) *
      glm::rotate(glm::mat4(1.0f), glm::radians(degree), {0.0f, 0.0f, 1.0f}) *
      glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

  DrawQuad(transform, texture, tiling_factor, tint_color);
}

void Renderer2D::DrawRotatedQuad(
    const glm::vec2& position, const glm::vec2& size, float degree,
    const Ref<SubTexture2D>& subtexture, float tiling_factor /*= 1.0f*/,
    const glm::vec4& tint_color /*= glm::vec4(1.0f)*/) {
  DrawRotatedQuad({position.x, position.y, 0.0f}, size, degree, subtexture,
                  tiling_factor, tint_color);
}

void Renderer2D::DrawRotatedQuad(
    const glm::vec3& position, const glm::vec2& size, float degree,
    const Ref<SubTexture2D>& subtexture, float tiling_factor /*= 1.0f*/,
    const glm::vec4& tint_color /*= glm::vec4(1.0f)*/) {
  HM_PROFILE_FUNCTION();
  if (kData.quad_index_count >= Renderer2DData::max_indices) NextBatch();
  constexpr size_t quad_vertex_count = 4;

  const glm::vec2* texture_coords = subtexture->GetTexCoords();
  const Ref<Texture2D> texture = subtexture->GetTexture();

  // 查询纹理是否存在
  float texture_idx = 0.0f;
  for (uint32_t i = 1; i < kData.texture_slot_idx; ++i) {
    if (*kData.texture_slots[i] == *texture) {
      texture_idx = (float)i;
      break;
    }
  }
  // 纹理不存在则添加纹理
  if (texture_idx == 0.0f) {
    if (kData.texture_slot_idx >= Renderer2DData::max_texture_slots)
      NextBatch();
    texture_idx = (float)kData.texture_slot_idx;
    kData.texture_slots[kData.texture_slot_idx] = texture;
    kData.texture_slot_idx++;
  }

  glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), position) *
      glm::rotate(glm::mat4(1.0f), glm::radians(degree), {0.0f, 0.0f, 1.0f}) *
      glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

  for (size_t i = 0; i < quad_vertex_count; i++) {
    kData.quad_vertex_buffer_ptr->position =
        transform * kData.quad_vertex_position[i];
    kData.quad_vertex_buffer_ptr->color = tint_color;
    kData.quad_vertex_buffer_ptr->tex_cood = texture_coords[i];
    kData.quad_vertex_buffer_ptr->tex_idx = texture_idx;
    kData.quad_vertex_buffer_ptr->tiling_factor = tiling_factor;
    kData.quad_vertex_buffer_ptr++;
  }

  kData.quad_index_count += 6;
  kData.stats.quad_count++;

}

void Renderer2D::DrawSprite(const glm::mat4& transform,
                            SpriteRendererComponent& src, int entity_id) {
  HM_PROFILE_FUNCTION();
  if (src.texture)
    DrawQuad(transform, src.texture, src.tiling_factor, src.color, entity_id);
  else
    DrawQuad(transform, src.color, entity_id);
}

void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color,
                            float thickness /*= 1.0f*/, float fade /*= 0.005f*/,
                            int entityID /*= -1*/) {
  HM_PROFILE_FUNCTION();

  // TODO: implement for circles
  // if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
  // 	NextBatch();
  constexpr size_t circlr_vertex_count = 4;
  for (size_t i = 0; i < circlr_vertex_count; i++) {
    kData.circle_vertex_buffer_ptr->world_position =
        transform * kData.quad_vertex_position[i];
    kData.circle_vertex_buffer_ptr->local_position =
       kData.quad_vertex_position[i] * 2.0f;
    kData.circle_vertex_buffer_ptr->color = color;
    kData.circle_vertex_buffer_ptr->thickness = thickness;
    kData.circle_vertex_buffer_ptr->fade = fade;
    kData.circle_vertex_buffer_ptr->entity_id = entityID;
    kData.circle_vertex_buffer_ptr++;
  }

  kData.circle_index_count += 6;

  kData.stats.quad_count++;
}

void Renderer2D::DrawLine(const glm::vec3& p0, glm::vec3& p1,
                          const glm::vec4& color, int entityID /*= -1*/) {
  kData.line_vertex_buffer_ptr->position = p0;
  kData.line_vertex_buffer_ptr->color = color;
  kData.line_vertex_buffer_ptr->entity_id = entityID;
  kData.line_vertex_buffer_ptr++;

  kData.line_vertex_buffer_ptr->position = p1;
  kData.line_vertex_buffer_ptr->color = color;
  kData.line_vertex_buffer_ptr->entity_id = entityID;
  kData.line_vertex_buffer_ptr++;

  kData.line_index_count += 2;
}
// the position is the center
void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size,
                          const glm::vec4& color, int entityID /*= -1*/) {
  glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f,
                           position.y - size.y * 0.5f, position.z);
  glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f,
                           position.y - size.y * 0.5f, position.z);
  glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f,
                           position.y + size.y * 0.5f, position.z);
  glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f,
                           position.y + size.y * 0.5f, position.z);

  DrawLine(p0, p1, color);
  DrawLine(p1, p2, color);
  DrawLine(p2, p3, color);
  DrawLine(p3, p0, color);
}

void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color,
                          int entityID /*= -1*/) {
  glm::vec3 lineVertices[4];
  for (size_t i = 0; i < 4; i++)
    lineVertices[i] = transform * kData.quad_vertex_position[i];

  DrawLine(lineVertices[0], lineVertices[1], color);
  DrawLine(lineVertices[1], lineVertices[2], color);
  DrawLine(lineVertices[2], lineVertices[3], color);
  DrawLine(lineVertices[3], lineVertices[0], color);
}

float Renderer2D::GetLineWidth() { return kData.line_width; }

void Renderer2D::SetLineWidth(float width) { kData.line_width = width; }

void Renderer2D::ResetStats() { memset(&kData.stats, 0, sizeof(Statistics)); }

Renderer2D::Statistics Renderer2D::GetStats() { return kData.stats; }

}// namespace hammer
