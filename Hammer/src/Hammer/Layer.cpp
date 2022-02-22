#include "hmpch.h"
#include "Layer.h"

namespace hammer {
Layer::Layer(const std::string& debug_name) : debug_name_(debug_name) {}
Layer::~Layer() {}
}  // namespace hammer