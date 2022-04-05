#include "SceneHierarchyPanel.h"
#include <cstring>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "glm/gtc/type_ptr.hpp"
/* The Microsoft C++ compiler is non-compliant with the C++ standard and needs
 * the following definition to disable a security warning on std::strncpy().
 */
#ifdef _MSVC_LANG
#define _CRT_SECURE_NO_WARNINGS
#endif

namespace hammer {
extern const std::filesystem::path global_asset_path;
SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context) {
  SetContext(context);
}

void SceneHierarchyPanel::SetContext(const Ref<Scene>& context) {
  scene_context_ = context;
  entity_selection_context_ = {};
}

void SceneHierarchyPanel::OnImGuiRender() {
  ImGui::Begin("Scene Hierarchy");
  
  if (scene_context_) {
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
  }
  ImGui::End();// Scene Hierarchy

  ImGui::Begin("Properties");
  if (entity_selection_context_) {
    DrawComponents(entity_selection_context_);
  }
  ImGui::End();// Properties
}

void SceneHierarchyPanel::SetSelectedEntity(Entity entity) {
  entity_selection_context_ = entity;
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
      ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
      ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
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
  // imgui show tag in HierarchyPanel
  if (entity.HasComponent<TagComponent>()) {
    auto& tag = entity.GetComponent<TagComponent>().tag;

    // ImGui::InputText can be changed,so we need a str buffer with enough capacity
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    std::strncpy(buffer, tag.c_str(), sizeof(buffer));
    if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
      tag = std::string(buffer);
  }
  // AddComponent operation in HierarchyPanel
  ImGui::SameLine();
  ImGui::PushItemWidth(-1);

  if (ImGui::Button("Add Component")) ImGui::OpenPopup("AddComponent");
  if (ImGui::BeginPopup("AddComponent")) {
    if (!entity_selection_context_.HasComponent<CameraComponent>()) {
      if (ImGui::MenuItem("Camera")) {
        entity_selection_context_.AddComponent<CameraComponent>();
        ImGui::CloseCurrentPopup();
      }
    }

    if (!entity_selection_context_.HasComponent<SpriteRendererComponent>()) {
      if (ImGui::MenuItem("Sprite Renderer")) {
        entity_selection_context_.AddComponent<SpriteRendererComponent>();
        ImGui::CloseCurrentPopup();
      }
    }

    if (!entity_selection_context_.HasComponent<CircleRendererComponent>()) {
      if (ImGui::MenuItem("Circle Renderer")) {
        entity_selection_context_.AddComponent<CircleRendererComponent>();
        ImGui::CloseCurrentPopup();
      }
    }
    
    if (!entity_selection_context_.HasComponent<Rigidbody2DComponent>()) {
      if (ImGui::MenuItem("Rigidbody 2D")) {
        entity_selection_context_.AddComponent<Rigidbody2DComponent>();
        ImGui::CloseCurrentPopup();
      }
    }

    if (!entity_selection_context_.HasComponent<BoxCollider2DComponent>()) {
      if (ImGui::MenuItem("BoxCollider 2D")) {
        entity_selection_context_.AddComponent<BoxCollider2DComponent>();
        ImGui::CloseCurrentPopup();
      }
    }
    if (!entity_selection_context_.HasComponent<CircleCollider2DComponent>()) {
      if (ImGui::MenuItem("CircleCollider 2D")) {
        entity_selection_context_.AddComponent<CircleCollider2DComponent>();
        ImGui::CloseCurrentPopup();
      }
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
  // imgui show Sprite properties
  DrawComponent<SpriteRendererComponent>(
      "Sprite Renderer", entity, [](auto& component) {
        ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
        ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
        if (ImGui::BeginDragDropTarget()) {
          if (const ImGuiPayload* payload =
                  ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
            const wchar_t* path = (const wchar_t*)payload->Data;
            std::filesystem::path texturePath =
                std::filesystem::path(global_asset_path) / path;
            Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
            if (texture->IsLoaded()) {
              component.texture = texture;
            } else {
              HM_WARN("Could not load texture {0}",
                      texturePath.filename().string());
            }
          }
          ImGui::EndDragDropTarget();
        }
        ImGui::DragFloat("Tiling Factor", &component.tiling_factor, 0.1f, 0.0f,
                         100.0f);
      });

  DrawComponent<CircleRendererComponent>(
      "Circle Renderer", entity, [](auto& component) {
        ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
        ImGui::DragFloat("Thickness", &component.thickness, 0.025f, 0.0f, 1.0f);
        ImGui::DragFloat("Fade", &component.fade, 0.00025f, 0.0f, 1.0f);
      });

  DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity,
                                      [](auto& component) {
        const char* bodyTypeStrings[] = {"Static", "Dynamic", "Kinematic"};
        const char* currentBodyTypeString =
            bodyTypeStrings[(int)component.type];
        if (ImGui::BeginCombo("Body Type", currentBodyTypeString)) {
          for (int i = 0; i < 2; i++) {
            bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
            if (ImGui::Selectable(bodyTypeStrings[i], isSelected)) {
              currentBodyTypeString = bodyTypeStrings[i];
              component.type = (Rigidbody2DComponent::BodyType)i;
            }

            if (isSelected) ImGui::SetItemDefaultFocus();
          }

          ImGui::EndCombo();
        }
        ImGui::Checkbox("Fixed Rotation", &component.fixed_rotation);
  });

  DrawComponent<BoxCollider2DComponent>(
      "BoxCollider 2D", entity, [](auto& component) {
        ImGui::DragFloat2("Offset", glm::value_ptr(component.offset));
        ImGui::DragFloat2("Size", glm::value_ptr(component.size));
        ImGui::DragFloat("Density", &component.density, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Friction", &component.friction, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Restitution", &component.restitution, 0.01f, 0.0f,
                         1.0f);
        ImGui::DragFloat("Restitution Threshold",
                         &component.restitution_threshold, 0.01f, 0.0f);
      });

  DrawComponent<CircleCollider2DComponent>(
      "CircleCollider 2D", entity, [](auto& component) {
        ImGui::DragFloat2("Offset", glm::value_ptr(component.offset));
        ImGui::DragFloat("Radius", &component.radius);
        ImGui::DragFloat("Density", &component.density, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Friction", &component.friction, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Restitution", &component.restitution, 0.01f, 0.0f,
                         1.0f);
        ImGui::DragFloat("Restitution Threshold",
                         &component.restitution_threshold, 0.01f, 0.0f);
      });
}

}  // namespace hammer