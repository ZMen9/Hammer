#pragma once

namespace hammer {
enum class RenderAPI { 
	None = 0,
	OpenGL = 1
};

class Renderer {
 public:
  inline static RenderAPI GetAPI() { return render_api_; }
 private: 
	static RenderAPI render_api_;
};

}  // namespace hammer