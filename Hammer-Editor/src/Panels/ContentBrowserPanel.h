#pragma once
#include <filesystem>
#include "Hammer/Renderer/Texture.h"
namespace hammer {
class ContentBrowserPanel {
 public:
  ContentBrowserPanel();
  void OnImGuiRender();

 private:
  std::filesystem::path current_directory_;
  Ref<Texture2D> directory_icon_;
  Ref<Texture2D> file_icon_;
};
}
