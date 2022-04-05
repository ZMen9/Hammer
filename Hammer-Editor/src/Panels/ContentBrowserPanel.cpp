#include "hmpch.h"
#include "ContentBrowserPanel.h"
#include <imgui/imgui.h>
namespace hammer {
extern const std::filesystem::path global_asset_path = "assets";
ContentBrowserPanel::ContentBrowserPanel()
    : current_directory_(global_asset_path) {
  directory_icon_ = Texture2D::Create("resources/icons/contentbrowser/DirectoryIcon.png");
  file_icon_ = Texture2D::Create("resources/icons/contentbrowser/FileIcon.png");
}

void ContentBrowserPanel::OnImGuiRender() {
  ImGui::Begin("Content Browser");
  if (current_directory_ != std::filesystem::path(global_asset_path)) {
    if (ImGui::Button(" < "))
      current_directory_ = current_directory_.parent_path();
  }
  static float padding = 16.0f;
  static float thumbnailSize = 120.0f;
  float cellSize = thumbnailSize + padding;

  float panelWidth = ImGui::GetContentRegionAvail().x;
  int columnCount = (int)(panelWidth / cellSize);
  if (columnCount < 1) columnCount = 1;
  ImGui::Columns(columnCount, 0, false);

  for (auto& directory_entry :
       std::filesystem::directory_iterator(current_directory_)) {
    const auto& path = directory_entry.path();
    auto relative_path = std::filesystem::relative(path, global_asset_path);
    // not show the relative path,just filename
    std::string filename_string = relative_path.filename().string();
    ImGui::PushID(filename_string.c_str());
    Ref<Texture2D> icon =
        directory_entry.is_directory() ? directory_icon_ : file_icon_;
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    // Show Icon
    ImGui::ImageButton((ImTextureID)icon->GetRenderID(),
                       {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});
    // DragDrop
    if (ImGui::BeginDragDropSource()) {
      const wchar_t* itemPath = relative_path.c_str();
      ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath,
                                (wcslen(itemPath) + 1) * sizeof(wchar_t));
      ImGui::EndDragDropSource();
    }
    ImGui::PopStyleColor();

    if (ImGui::IsItemHovered() &&
        ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
      if (directory_entry.is_directory())
        current_directory_ /= path.filename();
    }
    ImGui::TextWrapped(filename_string.c_str());
    ImGui::NextColumn();
    ImGui::PopID();
  }
  ImGui::Columns(1);
  ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
  ImGui::SliderFloat("Padding", &padding, 0, 32);
  // TODO: status bar
  ImGui::End();
}

}