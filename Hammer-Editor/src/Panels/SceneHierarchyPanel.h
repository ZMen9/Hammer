#pragma once
#include "Hammer.h"

namespace hammer {
class SceneHierarchyPanel {
 public:
  SceneHierarchyPanel() = default;
  SceneHierarchyPanel(const Ref<Scene>& context);

  void SetContext(const Ref<Scene>& context);

  void OnImGuiRender();

 private:
  void DrawEntityNode(Entity entity);
  void DrawComponents(Entity entity);

 private:
  Ref<Scene> scene_context_ = nullptr;
  Entity entity_selection_context_;
};
}  // namespace hammer