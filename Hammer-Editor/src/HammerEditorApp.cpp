#include <Hammer.h>
#include <Hammer/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace hammer {
class HammerEditor : public Application {
 public:
  HammerEditor(ApplicationCommandLineArgs args) 
      : Application("Hammer Editor", args) {
    PushLayer(new EditorLayer());
  }
  ~HammerEditor(){};
};

Application* CreateApplication(ApplicationCommandLineArgs args) {
  return new HammerEditor(args);
}
}  // namespace hammer
