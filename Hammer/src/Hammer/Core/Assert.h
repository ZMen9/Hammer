#pragma once
#include "Hammer/Core/Base.h"
#include "Hammer/Core/Log.h"
#include <filesystem>

#ifdef HM_ENABLE_ASSERTS

// Alternatively we could use the same "default" message for both "WITH_MSG" and
// "NO_MSG" and provide support for custom formatting by concatenating the
// formatting string instead of having the format inside the default message
#define HM_INTERNAL_ASSERT_IMPL(type, check, msg, ...) \
  {                                                    \
    if (!(check)) {                                    \
      HM##type##ERROR(msg, __VA_ARGS__);               \
      HM_DEBUGBREAK();                                 \
    }                                                  \
  }
#define HM_INTERNAL_ASSERT_WITH_MSG(type, check, ...) \
  HM_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define HM_INTERNAL_ASSERT_NO_MSG(type, check)                                 \
  HM_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}",    \
                          HM_STRINGIFY_MACRO(check),                           \
                          std::filesystem::path(__FILE__).filename().string(), \
                          __LINE__)

#define HM_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define HM_INTERNAL_ASSERT_GET_MACRO(...)            \
  HM_EXPAND_MACRO(HM_INTERNAL_ASSERT_GET_MACRO_NAME( \
      __VA_ARGS__, HM_INTERNAL_ASSERT_WITH_MSG, HM_INTERNAL_ASSERT_NO_MSG))

// Currently accepts at least the condition and one additional parameter (the
// message) being optional
#define HM_ASSERT(...) \
  HM_EXPAND_MACRO(HM_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__))
#define HM_CORE_ASSERT(...) \
  HM_EXPAND_MACRO(          \
      HM_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__))
#else
#define HM_ASSERT(...)
#define HM_CORE_ASSERT(...)
#endif  // HM_ENABLE_ASSERTS