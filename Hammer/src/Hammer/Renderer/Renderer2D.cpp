#include "hmpch.h"
#include "Hammer/Renderer/Renderer2D.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Hammer/Renderer/VertexArray.h"
#include "Hammer/Renderer/Shader.h"
#include "Hammer/Renderer/RenderCommand.h"

namespace hammer {

struct QuadVertex {
  glm::vec3 position;
  glm::vec4 color;
  glm::vec2 tex_cood;
  // TODO: tex_id
  float tex_idx;
  float tiling_factor;
};

struct Renderer2DData {
  static const uint32_t max_quads = 20000;
  static const uint32_t max_vertices = max_quads * 4;
  static const uint32_t max_indices = max_quads * 6;
  static const uint32_t max_texture_slots = 32;  // TODO: render caps

  Ref<VertexArray> quad_vertex_array;
  Ref<VertexBuffer> quad_vertex_buffer;
  Ref<Shader> texture_shader;
  Ref<Texture2D> white_texture;

  uint32_t quad_index_count = 0;
  QuadVertex* quad_vertex_buffer_base = nullptr;
  QuadVertex* quad_vertex_buffer_ptr = nullptr;

  std::array<Ref<Texture2D>, max_texture_slots> texture_slots;
  uint32_t texture_slot_idx = 1;// 0 = white texture

  glm::vec4 quad_vertex_position[4];
  
  Renderer2D::Statistics stats;
};

static Renderer2DData kData;

void Renderer2D::Init() {
  HM_PROFILE_FUNCTION();
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

  // texture and texture shader
  kData.white_texture = Texture2D::Create(1, 1);
  uint32_t white_texturedata = 0xffffffff;
  kData.white_texture->SetData(&white_texturedata, sizeof(uint32_t));

  int32_t samplers[kData.max_texture_slots];
  for (uint32_t i = 0; i < kData.max_texture_slots; i++) {
    samplers[i] = i;
  }
  // 设置纹理着色器
  kData.texture_shader = Shader::Create("assets/shaders/Texture.glsl");
  kData.texture_shader->Bind();
  kData.texture_shader->SetIntArray("u_Textures", samplers,
                                    kData.max_texture_slots);
  // set all texture slots to 0
  kData.texture_slots[0] = kData.white_texture;

  // 设置矩形四个顶点初始位置
  kData.quad_vertex_position[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
  kData.quad_vertex_position[1] = { 0.5f, -0.5f, 0.0f, 1.0f};
  kData.quad_vertex_position[2] = { 0.5f,  0.5f, 0.0f, 1.0f};
  kData.quad_vertex_position[3] = {-0.5f,  0.5f, 0.0f, 1.0f};
}

void Renderer2D::Shutdown() {
  HM_PROFILE_FUNCTION();

  delete[] kData.quad_vertex_buffer_base;

}

void Renderer2D::BeginScene(const OrthographicCamera& camera) {
  HM_PROFILE_FUNCTION();
  kData.texture_shader->Bind();
  kData.texture_shader->SetMat4("u_ViewProjection",
                                camera.view_projection_matrix());

  kData.quad_index_count = 0;
  kData.quad_vertex_buffer_ptr = kData.quad_vertex_buffer_base;

  kData.texture_slot_idx = 1;
}

void Renderer2D::EndScene() { 
  HM_PROFILE_FUNCTION();
  uint32_t data_size = (uint32_t)( (uint8_t*)kData.quad_vertex_buffer_ptr -
                       (uint8_t*)kData.quad_vertex_buffer_base );
  kData.quad_vertex_buffer->SetData(kData.quad_vertex_buffer_base, data_size);

  Flush();
}

void Renderer2D::Flush() {
  if (kData.quad_index_count == 0) return;

  // Bind textures
  for (uint32_t i = 0; i < kData.texture_slot_idx; i++)
    kData.texture_slots[i]->Bind(i);

  RenderCommand::DrawIndexed(kData.quad_vertex_array, kData.quad_index_count);
  kData.stats.draw_calls++;
}

void Renderer2D::FlushAndReset() {
  EndScene();
  kData.quad_index_count = 0;
  kData.quad_vertex_buffer_ptr = kData.quad_vertex_buffer_base;
  kData.texture_slot_idx = 1;
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,
                          const glm::vec4& color) {
  DrawQuad({position.x, position.y, 0.0f}, size, color);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size,
                          const glm::vec4& color) {
  HM_PROFILE_FUNCTION();
  if (kData.quad_index_count >= Renderer2DData::max_indices) FlushAndReset();

  const float texture_idx = 0.0f;// white texture
  constexpr size_t quad_vertex_count = 4;
  constexpr glm::vec2 texture_coords[] = {
      {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
  const float tiling_factor = 1.0f;


  glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                        glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

  for (size_t i = 0; i < quad_vertex_count; i++) {
    kData.quad_vertex_buffer_ptr->position =
        transform * kData.quad_vertex_position[i];
    kData.quad_vertex_buffer_ptr->color = color;
    kData.quad_vertex_buffer_ptr->tex_cood = texture_coords[i];
    kData.quad_vertex_buffer_ptr->tex_idx = texture_idx;
    kData.quad_vertex_buffer_ptr->tiling_factor = tiling_factor;
    kData.quad_vertex_buffer_ptr++;
  }
  kData.quad_index_count += 6;
  kData.stats.quad_count++;
}


void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,
                          const Ref<Texture2D>& texture, float tiling_factor,
                          const glm::vec4& tint_color) {
  DrawQuad({position.x, position.y, 0.0f}, size, texture, tiling_factor,
           tint_color);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size,
                          const Ref<Texture2D>& texture, float tiling_factor,
                          const glm::vec4& tint_color) {
  HM_PROFILE_FUNCTION();
  if (kData.quad_index_count >= Renderer2DData::max_indices) FlushAndReset();

  constexpr size_t quad_vertex_count = 4;
  constexpr glm::vec2 texture_coords[] = {
      {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};

  // 查询纹理是否存在
  float texture_idx = 0.0f;
  for (uint32_t i = 1; i < kData.texture_slot_idx; ++i) {
    if (*kData.texture_slots[i].get() == *texture.get()) {
      texture_idx = (float)i;
      break;
    }
  }
  // 纹理不存在则添加纹理
  if (texture_idx == 0.0f) {
    if (kData.texture_slot_idx >= Renderer2DData::max_texture_slots) FlushAndReset();
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
  if (kData.quad_index_count >= Renderer2DData::max_indices) FlushAndReset();

  constexpr size_t quad_vertex_count = 4;
  const glm::vec2* texture_coords = subtexture->GetTexCoords();
  const Ref<Texture2D> texture = subtexture->GetTexture();

  // 查询纹理是否存在
  float texture_idx = 0.0f;
  for (uint32_t i = 1; i < kData.texture_slot_idx; ++i) {
    if (*kData.texture_slots[i].get() == *texture.get()) {
      texture_idx = (float)i;
      break;
    }
  }
  // 纹理不存在则添加纹理
  if (texture_idx == 0.0f) {
    if (kData.texture_slot_idx >= Renderer2DData::max_texture_slots)
      FlushAndReset();
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
  if (kData.quad_index_count >= Renderer2DData::max_indices) FlushAndReset();
  const float texture_idx = 0.0f;  // White Texture
  const float tiling_factor = 1.0f;
  constexpr size_t quad_vertex_count = 4;
  constexpr glm::vec2 texture_coords[] = {
      {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};

  glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), position) *
      glm::rotate(glm::mat4(1.0f), glm::radians(degree), {0.0f, 0.0f, 1.0f}) *
      glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

  for (size_t i = 0; i < quad_vertex_count; i++) {
    kData.quad_vertex_buffer_ptr->position =
        transform * kData.quad_vertex_position[i];
    kData.quad_vertex_buffer_ptr->color = color;
    kData.quad_vertex_buffer_ptr->tex_cood = texture_coords[i];
    kData.quad_vertex_buffer_ptr->tex_idx = texture_idx;
    kData.quad_vertex_buffer_ptr->tiling_factor = tiling_factor;
    kData.quad_vertex_buffer_ptr++;
  }

  kData.quad_index_count += 6;
  kData.stats.quad_count++;
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
  if (kData.quad_index_count >= Renderer2DData::max_indices) FlushAndReset();
  constexpr size_t quad_vertex_count = 4;
  constexpr glm::vec2 texture_coords[] = {
      {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};

  // 查询纹理是否存在
  float texture_idx = 0.0f;
  for (uint32_t i = 1; i < kData.texture_slot_idx; ++i) {
    if (*kData.texture_slots[i].get() == *texture.get()) {
      texture_idx = (float)i;
      break;
    }
  }
  // 纹理不存在则添加纹理
  if (texture_idx == 0.0f) {
    if (kData.texture_slot_idx >= Renderer2DData::max_texture_slots)
      FlushAndReset();
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
  if (kData.quad_index_count >= Renderer2DData::max_indices) FlushAndReset();
  constexpr size_t quad_vertex_count = 4;

  const glm::vec2* texture_coords = subtexture->GetTexCoords();
  const Ref<Texture2D> texture = subtexture->GetTexture();

  // 查询纹理是否存在
  float texture_idx = 0.0f;
  for (uint32_t i = 1; i < kData.texture_slot_idx; ++i) {
    if (*kData.texture_slots[i].get() == *texture.get()) {
      texture_idx = (float)i;
      break;
    }
  }
  // 纹理不存在则添加纹理
  if (texture_idx == 0.0f) {
    if (kData.texture_slot_idx >= Renderer2DData::max_texture_slots)
      FlushAndReset();
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

void Renderer2D::ResetStats() { memset(&kData.stats, 0, sizeof(Statistics)); }

Renderer2D::Statistics Renderer2D::GetStats() { return kData.stats; }

}// namespace hammer