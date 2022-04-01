#include <Hammer.h>
#include <Hammer/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace hammer {
class HammerEditor : public Application {
 public:
  HammerEditor() {
    PushLayer(new EditorLayer());
  }
  ~HammerEditor(){};
};

Application* CreateApplication() { return new HammerEditor(); }
}  // namespace hammer
