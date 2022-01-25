#include "Hammer.h"

class Sandbox : public Hammer::Application {
 public:
  Sandbox(){};
  ~Sandbox(){};
};

Hammer::Application* Hammer::CreateApplication() { return new Sandbox(); }