#pragma once
#include <glm/glm.hpp>
namespace hammer::math {
bool DecomposeTranform(const glm::mat4& transform, glm::vec3& out_translation,
                       glm::vec3& out_rotation, glm::vec3& out_scale);
}