#include "hmpch.h"
#include "Hammer/Utils/PlatformUtils.h"
#include <commdlg.h>
#include "Hammer/Core/Application.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW//glfw3native.h>
// You don't have to browse your whole computer to go to the assets/scenes folder anymore.
// only on Windows
namespace hammer {
std::string FileDialogs::OpenFile(const char* filter) {
  OPENFILENAMEA ofn; // common dialog box structure
  CHAR szFile[260] = {0}; // if using TCHAR macros
  CHAR currentDir[256] = {0};
  // Initialize OPENFILENAMEA
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = glfwGetWin32Window(
      (GLFWwindow*)Application::instance().window().GetNativeWindow());
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile);
  if (GetCurrentDirectoryA(256, currentDir)) ofn.lpstrInitialDir = currentDir;
  ofn.lpstrFilter = filter;
  ofn.nFilterIndex = 1;
  // OFN_NOCHANGEDIR is important 
  // must be used,or GetOpenFileNameA will change the working dir
  // A is ascii
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
  if (GetOpenFileNameA(&ofn) == TRUE) 
    return ofn.lpstrFile;
  return std::string();
}
std::string FileDialogs::SaveFile(const char* filter) {
  OPENFILENAMEA ofn;
  CHAR szFile[260] = {0};
  CHAR currentDir[256] = {0};
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = glfwGetWin32Window(
      (GLFWwindow*)Application::instance().window().GetNativeWindow());
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile);
  if (GetCurrentDirectoryA(256, currentDir)) ofn.lpstrInitialDir = currentDir;
  ofn.lpstrFilter = filter;
  ofn.nFilterIndex = 1;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
  // Sets the default extension by extracting it from the filter
  // Added default extension string when saving a scene
  // If the extension is not specified,the file was not saved with a.hammer extension
  ofn.lpstrDefExt = std::strchr(filter, '\0') + 1;
  if (GetSaveFileNameA(&ofn) == TRUE) 
    return ofn.lpstrFile;
  return std::string();
}

}