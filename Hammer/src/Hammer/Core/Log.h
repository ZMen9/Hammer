#pragma once

#include "Hammer/Core/Base.h"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>// must place here first before ostr.h
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace hammer {
class Log {
 public:
  static void Init();

  inline static Ref<spdlog::logger>& core_logger() {
    return core_logger_;
  }

  inline static Ref<spdlog::logger>& client_logger() {
    return client_logger_;
  }
 private:
  static Ref<spdlog::logger> core_logger_;
  static Ref<spdlog::logger> client_logger_;
};
}  // namespace hammer

template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector) {
  return os << glm::to_string(vector);
}

template <typename OStream, glm::length_t C, glm::length_t R, typename T,
          glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix) {
  return os << glm::to_string(matrix);
}

template <typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion) {
  return os << glm::to_string(quaternion);
}

// Core log macros
#define HM_CORE_TRACE(...)  ::hammer::Log::core_logger()->trace(__VA_ARGS__)
#define HM_CORE_INFO(...)   ::hammer::Log::core_logger()->info(__VA_ARGS__)
#define HM_CORE_WARN(...)   ::hammer::Log::core_logger()->warn(__VA_ARGS__)
#define HM_CORE_ERROR(...)  ::hammer::Log::core_logger()->error(__VA_ARGS__)
#define HM_CORE_CRITICAL(...) \
  ::hammer::Log::core_logger()->critical(__VA_ARGS__)

// Client log macros          
#define HM_TRACE(...)       ::hammer::Log::client_logger()->trace(__VA_ARGS__)
#define HM_INFO(...)        ::hammer::Log::client_logger()->info(__VA_ARGS__)
#define HM_WARN(...)        ::hammer::Log::client_logger()->warn(__VA_ARGS__)
#define HM_ERROR(...)       ::hammer::Log::client_logger()->error(__VA_ARGS__)
#define HM_CRITICAL(...)    ::hammer::Log::client_logger()->critical(__VA_ARGS__)