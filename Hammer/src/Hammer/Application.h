#pragma once
#include "Core.h"
namespace Hammer {
class HAMMER_API Application {
 public:
  Application();
  virtual ~Application();
  void Run();
};

// Should be defind by client application
Application* CreateApplication();

}  // namespace Hammer
