#pragma once
#include "Hammer/Renderer/Camera.h"
#include "Hammer/Renderer/OrthographicCamera.h"
#include "Hammer/Renderer/Texture.h"
#include "Hammer/Renderer/SubTexture2D.h"
namespace hammer {
class Renderer2D {
 public:
  static void Init();
  static void Shutdown();

  static void BeginScene(const Camera& camera, const glm::mat4& transform);
  static void BeginScene(const OrthographicCamera& camera);  // TODO: Remove
  static void EndScene();
  static void Flush();

  // Primitives
  static void DrawQuad(const glm::vec2& position, const glm::vec2& size,
                       const glm::vec4& color);
  static void DrawQuad(const glm::vec3& position, const glm::vec2& size,
                       const glm::vec4& color);
  // with texture
  static void DrawQuad(const glm::vec2& position, const glm::vec2& size,
                       const Ref<Texture2D>& texture, float tiling_factor = 1.0f,
                       const glm::vec4& tint_color = glm::vec4(1.0f));
  static void DrawQuad(const glm::vec3& position, const glm::vec2& size,
                       const Ref<Texture2D>& texture, float tiling_factor = 1.0f,
                       const glm::vec4& tint_color = glm::vec4(1.0f));
  // with subtexture
  static void DrawQuad(const glm::vec2& position, const glm::vec2& size,
                       const Ref<SubTexture2D>& subtexture,
                       float tiling_factor = 1.0f,
                       const glm::vec4& tint_color = glm::vec4(1.0f));
  static void DrawQuad(const glm::vec3& position, const glm::vec2& size,
                       const Ref<SubTexture2D>& subtexture,
                       float tiling_factor = 1.0f,
                       const glm::vec4& tint_color = glm::vec4(1.0f));
  // for Components
  static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);
  static void DrawQuad(const glm::mat4& transform,
                       const Ref<Texture2D>& texture,
                       float tiling_factor = 1.0f,
                       const glm::vec4& tint_color = glm::vec4(1.0f));

  static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size,
                              float degree, const glm::vec4& color);

  static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size,
                              float degree, const glm::vec4& color);

  static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size,
                              float degree, const Ref<Texture2D>& texture,
                              float tiling_factor = 1.0f,
                              const glm::vec4& tint_color = glm::vec4(1.0f));

  static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size,
                              float degree, const Ref<Texture2D>& texture,
                              float tiling_factor = 1.0f,
                              const glm::vec4& tint_color = glm::vec4(1.0f));

  static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size,
                              float degree, const Ref<SubTexture2D>& subtexture,
                              float tiling_factor = 1.0f,
                              const glm::vec4& tint_color = glm::vec4(1.0f));

  static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size,
                              float degree, const Ref<SubTexture2D>& subtexture,
                              float tiling_factor = 1.0f,
                              const glm::vec4& tint_color = glm::vec4(1.0f));

  struct Statistics {
    uint32_t draw_calls = 0;
    uint32_t quad_count = 0;

    uint32_t GetTotalVertexCount() const { return quad_count * 4; }
    uint32_t GetTotalIndexCount() const { return quad_count * 6; }
  };
  static void ResetStats();
  static Statistics GetStats();

 private:
  static void StartBatch();
  static void NextBatch();
   
};
} // namespace hammer