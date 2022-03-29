#include "hmpch.h"
#include "Hammer/Renderer/SubTexture2D.h"

namespace hammer {

 SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture,
                           const glm::vec2& min_bound,
                           const glm::vec2& max_bound) 
    : texture_(texture){
  tex_coords[0] = {min_bound.x, min_bound.y};
  tex_coords[1] = {max_bound.x, min_bound.y};
  tex_coords[2] = {max_bound.x, max_bound.y};
  tex_coords[3] = {min_bound.x, max_bound.y};

 }

Ref<SubTexture2D> SubTexture2D::CreateUseCoords(Ref<Texture2D> texture,
                                                 const glm::vec2& coords,
                                                 const glm::vec2& cell_size,
                                                 const glm::vec2& sprite_size) {
   glm::vec2 min_bound = {(coords.x * cell_size.x) / texture->width(),
                          (coords.y * cell_size.y) / texture->heigth()};
   glm::vec2 max_bound = {
       ((coords.x + sprite_size.x) * cell_size.x) / texture->width(),
       ((coords.y + sprite_size.y) * cell_size.y) / texture->heigth()};

   return CreateRef<SubTexture2D>(texture, min_bound, max_bound);
 }

}