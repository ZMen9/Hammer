#include "hmpch.h"
#include "RenderCommand.h"
#include "PlatForm/OpenGL/OpenGLRendererAPI.h"


namespace hammer {
RendererAPI* RenderCommand::kRendererAPI = new OpenRendererAPI();
} //namespace hammer