#include "SceneHierarchyPanel.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "glm/gtc/type_ptr.hpp"

namespace hammer {
SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context) {
  SetContext(context);
}

void SceneHierarchyPanel::SetContext(const Ref<Scene>& context) {
  scene_context_ = context;
}

void SceneHierarchyPanel::OnImGuiRender() {
  ImGui::Begin("Scene Hierarchy");

  scene_context_->registry_.each([=](auto entityID) {
    Entity entity{entityID, scene_context_.get()};
    DrawEntityNode(entity);
  });

  if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
    entity_selection_context_ = {};

  if (ImGui::BeginPopupContextWindow(0, 1, false)) {
    if (ImGui::MenuItem("Create Empty Entity")) {
      scene_context_->CreateEntity("Empty Entity");
    }
    ImGui::EndPopup();
  }

  ImGui::End();// Scene Hierarchy

  ImGui::Begin("Properties");
  if (entity_selection_context_) {
    DrawComponents(entity_selection_context_);
  }
  ImGui::End();// Properties
}

void SceneHierarchyPanel::DrawEntityNode(Entity entity) {
  auto& tag = entity.GetComponent<TagComponent>().tag;
  ImGuiTreeNodeFlags flags =
      ((entity_selection_context_ == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
      ImGuiTreeNodeFlags_OpenOnArrow;
  flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
  bool opened =
      ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
  if (ImGui::IsItemClicked()) {
    entity_selection_context_ = entity;
  }

  bool entity_deleted = false;
  if (ImGui::BeginPopupContextItem()) {
    if (ImGui::MenuItem("Delete Entity")) entity_deleted = true;
    ImGui::EndPopup();
  }

  if (opened) {
    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
    if (opened) ImGui::TreePop();
    ImGui::TreePop();
  }

  if (entity_deleted) {
    scene_context_->DestroyEntity(entity);
    if (entity_selection_context_ == entity) {
      entity_selection_context_ = {};
    }

  }
}

static void DrawVec3Control(const std::string& label, glm::vec3& values,
                            float resetValue = 0.0f,
                            float columnWidth = 100.0f) {
  ImGuiIO& io = ImGui::GetIO();
  auto boldFont = io.Fonts->Fonts[0];
  ImGui::PushID(label.c_str());
  ImGui::Columns(2);
  // leftest column
  ImGui::SetColumnWidth(0, columnWidth);
  ImGui::Text(label.c_str());
  ImGui::NextColumn();
  // x,y,z of translation
  ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
  // button label:
  // this is from imgui internal
  float lineHeight =
      GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
  ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};
  // first item
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
  ImGui::PushFont(boldFont);
  // if the button is pressed:
  if(ImGui::Button("X", buttonSize)) values.x = resetValue;
  ImGui::PopFont();
  ImGui::PopStyleColor(3);

  ImGui::SameLine();
  ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
  ImGui::PopItemWidth();// pop the first item
  ImGui::SameLine();
  // second item
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
  ImGui::PushFont(boldFont);
  if (ImGui::Button("Y", buttonSize)) values.y = resetValue;
  ImGui::PopFont();
  ImGui::PopStyleColor(3);

  ImGui::SameLine();
  ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
  ImGui::PopItemWidth();
  ImGui::SameLine();
  // third item
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                        ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
  ImGui::PushFont(boldFont);
  if (ImGui::Button("Z", buttonSize)) values.z = resetValue;
  ImGui::PopFont();
  ImGui::PopStyleColor(3);

  ImGui::SameLine();
  ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
  ImGui::PopItemWidth();

  ImGui::PopStyleVar();

  ImGui::Columns(1);
  ImGui::PopID();
}
template<typename T, typename UIFunction>
static void DrawComponent(const std::string& name, Entity entity,
  UIFunction uiFunction) {
  const ImGuiTreeNodeFlags treeNodeFlags =
      ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap |
      ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth |
      ImGuiTreeNodeFlags_FramePadding;
  if (entity.HasComponent<T>()) {
    auto& component = entity.GetComponent<T>();
    ImVec2 content_region_available = ImGui::GetContentRegionAvail();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
    float lineHeight =
        GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImGui::Separator();
    bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags,
                                  name.c_str());
    ImGui::PopStyleVar();
    ImGui::SameLine(content_region_available.x - lineHeight * 0.5f);
    if (ImGui::Button("+", ImVec2{lineHeight, lineHeight})) {
      ImGui::OpenPopup("ComponentSettings");
    }
    bool removeComponent = false;
    if (ImGui::BeginPopup("ComponentSettings")) {
      if (ImGui::MenuItem("Remove component")) removeComponent = true;

      ImGui::EndPopup();
    }
    if (open) {
      uiFunction(component);
      ImGui::TreePop();
    }
    if (removeComponent) entity.RemoveComponent<T>();
  }
}

void SceneHierarchyPanel::DrawComponents(Entity entity) {
  // imgui show tag
  if (entity.HasComponent<TagComponent>()) {
    auto& tag = entity.GetComponent<TagComponent>().tag;

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    strcpy_s(buffer, sizeof(buffer), tag.c_str());
    if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
      tag = std::string(buffer);
  }
  // AddComponent operation
  ImGui::SameLine();
  ImGui::PushItemWidth(-1);

  if (ImGui::Button("Add Component")) ImGui::OpenPopup("AddComponent");
  if (ImGui::BeginPopup("AddComponent")) {
    if (ImGui::MenuItem("Camera")) {
      entity_selection_context_.AddComponent<CameraComponent>();
      ImGui::CloseCurrentPopup();
    }
    if (ImGui::MenuItem("Sprite Renderer")) {
      entity_selection_context_.AddComponent<SpriteRendererComponent>();
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
  ImGui::PopItemWidth();

  // imgui show transform properties
  DrawComponent<TransformComponent>("Transform", entity, [](auto& component) {
    DrawVec3Control("Translation", component.translation);
    glm::vec3 rotation = glm::degrees(component.rotation);
    DrawVec3Control("Rotation", rotation);
    component.rotation = glm::radians(rotation);
    DrawVec3Control("Scale", component.scale, 1.0f);
  });

  // imgui show Camera properties
  DrawComponent<CameraComponent>("Camera", entity, [](auto& component) {
    auto& camera = component.camera;

    ImGui::Checkbox("Primary", &component.primary);

    const char* projection_type_strings[] = {"Perspective", "Orthographic"};
    const char* current_projection_type_string =
        projection_type_strings[(int)camera.GetProjectionType()];

    if (ImGui::BeginCombo("Projection", current_projection_type_string)) {
      for (int i = 0; i < 2; i++) {
        bool is_selected =
            current_projection_type_string == projection_type_strings[i];
        if (ImGui::Selectable(projection_type_strings[i], is_selected)) {
          current_projection_type_string = projection_type_strings[i];
          camera.SetProjectionType((SceneCamera::ProjectionType)i);
        }

        if (is_selected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
    if (camera.GetProjectionType() ==
        SceneCamera::ProjectionType::Perspective) {
      float vertical_fov = glm::degrees(camera.GetPerspectiveVerticalFOV());
      if (ImGui::DragFloat("Vertical FOV", &vertical_fov))
        camera.SetPerspectiveVerticalFOV(glm::radians(vertical_fov));

      float persp_near = camera.GetPerspectiveNearClip();
      if (ImGui::DragFloat("Near Clip", &persp_near))
        camera.SetPerspectiveNearClip(persp_near);

      float persp_far = camera.GetPerspectiveFarClip();
      if (ImGui::DragFloat("Far Clip", &persp_far))
        camera.SetPerspectiveFarClip(persp_far);
    }

    if (camera.GetProjectionType() ==
        SceneCamera::ProjectionType::Orthographic) {
      float ortho_size = camera.GetOrthographicSize();
      if (ImGui::DragFloat("Size", &ortho_size))
        camera.SetOrthographicSize(ortho_size);

      float ortho_near = camera.GetOrthographicNearClip();
      if (ImGui::DragFloat("Near Clip", &ortho_near))
        camera.SetOrthographicNearClip(ortho_near);

      float ortho_far = camera.GetOrthographicFarClip();
      if (ImGui::DragFloat("Far Clip", &ortho_far))
        camera.SetOrthographicFarClip(ortho_far);

      ImGui::Checkbox("Fixed Aspect Ratio",
                      &component.fixed_aspect_ratio);
    }
  });

  DrawComponent<SpriteRendererComponent>(
      "Sprite Renderer", entity, [](auto& component) {
        ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
      });
}

}  // namespace hammer