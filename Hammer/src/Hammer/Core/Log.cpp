#include "hmpch.h"
#include "Hammer/Core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace hammer {
Ref<spdlog::logger> Log::core_logger_;
Ref<spdlog::logger> Log::client_logger_;

void Log::Init() {
  std::vector<spdlog::sink_ptr> log_sinks;
  log_sinks.emplace_back(
      std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
  log_sinks.emplace_back(
      std::make_shared<spdlog::sinks::basic_file_sink_mt>("Hammer.log", true));

  log_sinks[0]->set_pattern("%^[%T] %n: %v%$");
  log_sinks[1]->set_pattern("[%T] [%l] %n: %v");

  core_logger_ = std::make_shared<spdlog::logger>("Hammer", begin(log_sinks),
                                                  end(log_sinks));
  spdlog::register_logger(core_logger_);
  core_logger_->set_level(spdlog::level::trace);
  core_logger_->flush_on(spdlog::level::trace);

  client_logger_ = std::make_shared<spdlog::logger>("APP", begin(log_sinks),
                                                    end(log_sinks));
  spdlog::register_logger(client_logger_);
  client_logger_->set_level(spdlog::level::trace);
  client_logger_->set_level(spdlog::level::trace);

};
}  // namespace hammer
