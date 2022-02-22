#pragma once

#include <vector>

#include "Hammer/Core.h"
#include "Layer.h"

namespace hammer {
class HAMMER_API LayerStack {
 public:
  LayerStack();
  ~LayerStack();

  void PushLayer(Layer* layer);
  void PushOverlay(Layer* overlay);
  void PopLayer(Layer* layer);
  void PopOverlay(Layer* layer);

  std::vector<Layer*>::iterator begin() { return layers_.begin(); }
  std::vector<Layer*>::iterator end() { return layers_.end(); }
 private: 
  std::vector<Layer*> layers_;
  std::vector<Layer*>::iterator layer_insert_;
};
}  // namespace hammer