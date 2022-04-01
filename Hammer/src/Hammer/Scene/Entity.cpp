#include "hmpch.h"
#include "Hammer/Scene/Entity.h"
namespace hammer {
Entity::Entity(entt::entity handle, Scene* scene)
		: entity_handle_(handle), scene_(scene) {}
} // namespace hammer