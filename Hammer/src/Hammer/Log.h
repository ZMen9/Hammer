#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace hammer {
class HAMMER_API Log {
 public:
  static void Init();

  inline static std::shared_ptr<spdlog::logger>& core_logger() {
    return core_logger_;
  }

  inline static std::shared_ptr<spdlog::logger>& client_logger() {
    return client_logger_;
  }
 private:
  static std::shared_ptr<spdlog::logger> core_logger_;
  static std::shared_ptr<spdlog::logger> client_logger_;
};
}  // namespace hammer

// Core log macros
#define HM_CORE_TRACE(...)  ::hammer::Log::core_logger()->trace(__VA_ARGS__)
#define HM_CORE_INFO(...)   ::hammer::Log::core_logger()->info(__VA_ARGS__)
#define HM_CORE_WARN(...)   ::hammer::Log::core_logger()->warn(__VA_ARGS__)
#define HM_CORE_ERROR(...)  ::hammer::Log::core_logger()->error(__VA_ARGS__)
#define HM_CORE_FATAL(...)  ::hammer::Log::core_logger()->critical(__VA_ARGS__)

// Client log macros          
#define HM_TRACE(...)       ::hammer::Log::client_logger()->trace(__VA_ARGS__)
#define HM_INFO(...)        ::hammer::Log::client_logger()->info(__VA_ARGS__)
#define HM_WARN(...)        ::hammer::Log::client_logger()->warn(__VA_ARGS__)
#define HM_ERROR(...)       ::hammer::Log::client_logger()->error(__VA_ARGS__)
#define HM_FATAL(...)       ::hammer::Log::client_logger()->critical(__VA_ARGS__)