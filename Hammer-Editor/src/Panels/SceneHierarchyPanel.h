#pragma once
#include "Hammer.h"

namespace hammer {
class SceneHierarchyPanel {
 public:
  SceneHierarchyPanel() = default;
  SceneHierarchyPanel(const Ref<Scene>& context);

  void SetContext(const Ref<Scene>& context);

  void OnImGuiRender();

  Entity GetSelectedEntity() const { return entity_selection_context_; } 
  void SetSelectedEntity(Entity entity);
 private:
  void DrawEntityNode(Entity entity);
  void DrawComponents(Entity entity);

 private:
  Ref<Scene> scene_context_ = nullptr;
  Entity entity_selection_context_ = {};
};
}  // namespace hammer