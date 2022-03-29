#include "hmpch.h"
#include "Hammer/Core/Input.h"

#ifdef HM_PLATFORM_WINDOWS
	#include "PlatForm/Windows/WindowsInput.h"
#endif

namespace hammer {
Scope<Input> Input::kInstance = Input::Create();

Scope<Input> Input::Create() {
#ifdef HM_PLATFORM_WINDOWS
  return CreateScope<WindowsInput>();
  #else 
  HM_CORE_ASSERT(false, "Unknown platform!");
  return nullptr;

#endif  // HM_PLATFORM_WINDOWS
  
}
} // namespace hammer