#pragma once
#include "Hammer/Scene/Scene.h"

namespace hammer {
class SceneSerializer {
 public:
  SceneSerializer(const Ref<Scene> scene);
  
  void Serialize(const std::string& file_path);
  void SerializeRuntime(const std::string& file_path);

  bool Deserialize(const std::string& file_path);
  bool DeserializeRuntime(const std::string& file_path);

 private:
  Ref<Scene> scene_;
};

}