#pragma once
#include <memory>
// Platform detection using predefined macros

#ifdef _WIN32
/* Windows x64/x86 */
#ifdef _WIN64
/* Windows x64  */
#define HM_PLATFORM_WINDOWS
#else
/* Windows x86 */
#error "x86 Builds are not supported!"
#endif

#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>
/* TARGET_OS_MAC exists on all the platforms
 * so we must check all of them (in this order)
 * to ensure that we're running on MAC
 * and not some other Apple platform */
#if TARGET_IPHONE_SIMULATOR == 1
#error "IOS simulator is not supported!"
#elif TARGET_OS_IPHONE == 1
#define HZ_PLATFORM_IOS
#error "IOS is not supported!"
#elif TARGET_OS_MAC == 1
#define HZ_PLATFORM_MACOS
#error "MacOS is not supported!"
#else
#error "Unknown Apple platform!"
#endif
/* We also have to check __ANDROID__ before __linux__
 * since android is based on the linux kernel
 * it has __linux__ defined */
#elif defined(__ANDROID__)
#define HZ_PLATFORM_ANDROID
#error "Android is not supported!"
#elif defined(__linux__)
#define HZ_PLATFORM_LINUX
#error "Linux is not supported!"
#else
/* Unknown compiler/platform */
#error "Unknown platform!"

#endif  // End of platform detection


#ifdef HM_DEBUG
//
#if defined(HM_PLATFORM_WINDOWS)
  #define HM_DEBUGBREAK() __debugbreak()
#elif defined(HM_PLATFORM_LINUX)
  #include <signal.h>
  #define HM_DEBUGBREAK() raise(SIGTRAP)
#else
  #error "Platform doesn't support debugbreak yet!"
#endif
//
#define HM_ENABLE_ASSERTS
#else
#define HM_DEBUGBREAK()
#endif

#ifdef HM_ENABLE_ASSERTS
#define HM_ASSERT(x, ...)                             \
  {                                                   \
    if (!(x)) {                                       \
      HM_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
      HM_DEBUGBREAK();                                 \
    }                                                 \
  }
#define HM_CORE_ASSERT(x, ...)                             \
  {                                                        \
    if (!(x)) {                                            \
      HM_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
      HM_DEBUGBREAK();                                      \
    }                                                      \
  }
#else
#define HM_ASSERT(x, ...)
#define HM_CORE_ASSERT(x, ...)
#endif  // HM_ENABLE_ASSERTS

#define BIT(x) (1 << x)

#define HM_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace hammer {

template <typename T>
using Scope = std::unique_ptr<T>;
template <typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&&... agrs) {
  return std::make_unique<T>(std::forward<Args>(agrs)...);
}

template <typename T>
using Ref = std::shared_ptr<T>;
template <typename T ,typename... Args>
constexpr Ref<T> CreateRef(Args&&... args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}
}  // namespace hammer