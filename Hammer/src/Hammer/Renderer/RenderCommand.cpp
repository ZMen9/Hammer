#include "hmpch.h"
#include "Hammer/Renderer/RenderCommand.h"
#include "PlatForm/OpenGL/OpenGLRendererAPI.h"


namespace hammer {
Scope<RendererAPI> RenderCommand::kRendererAPI = RendererAPI::Create();
} //namespace hammer