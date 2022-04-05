#pragma once
#include <string>
namespace hammer {
class FileDialogs {
 public:
  // These return empty strings if canceled
  static std::string OpenFile(const char* filter);
  static std::string SaveFile(const char* filter);
};

}