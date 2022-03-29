#pragma once

#include <vector>

#include "Hammer/Core/Base.h"
#include "Hammer/Core/Layer.h"

namespace hammer {
class LayerStack {
 public:
  LayerStack() = default;
  ~LayerStack();

  void PushLayer(Layer* layer);
  void PushOverlay(Layer* overlay);
  void PopLayer(Layer* layer);
  void PopOverlay(Layer* layer);

  std::vector<Layer*>::iterator begin() { return layers_.begin(); }
  std::vector<Layer*>::iterator end() { return layers_.end(); }
  std::vector<Layer*>::reverse_iterator rbeing() { return layers_.rbegin(); }
  std::vector<Layer*>::reverse_iterator rend() { return layers_.rend(); }

  std::vector<Layer*>::const_iterator cbegin() { return layers_.cbegin(); }
  std::vector<Layer*>::const_iterator cend() { return layers_.cend(); }
  std::vector<Layer*>::const_reverse_iterator crbegin() {
    return layers_.crbegin();
  }
  std::vector<Layer*>::const_reverse_iterator crend() { return layers_.crend(); }
 private: 
  std::vector<Layer*> layers_;
  unsigned int layer_insert_index_ = 0;
};
}  // namespace hammer