#pragma once
#include <glm/glm.hpp>
#include "Hammer/Renderer/Texture.h"

namespace hammer {
class SubTexture2D {
 public:
  SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min_bound,
               const glm::vec2& max_bound);

  const Ref<Texture2D>& GetTexture() const { return texture_; }
  const glm::vec2* GetTexCoords() const { return tex_coords; }

  static Ref<SubTexture2D> CreateUseCoords(Ref<Texture2D> texture, 
                                           const glm::vec2& coords,
                                           const glm::vec2& cell_size,
                                           const glm::vec2& sprite_size = {1,1});
 private:
  Ref<Texture2D> texture_;
  glm::vec2 tex_coords[4];
};
}

