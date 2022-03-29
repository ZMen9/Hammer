#include "hmpch.h"
#include "Hammer/Core/Window.h"

#ifdef HM_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace hammer {
Scope<Window> Window::Create(const WindowProps& props) {
#ifdef HM_PLATFORM_WINDOWS
  return CreateScope<WindowsWindow>(props);
#else 
  HM_CORE_ASSERT(false, "Unknown platform!");
  return nullptr;
#endif
}
} // namespace hammer
