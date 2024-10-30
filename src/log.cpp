#include "log.hpp"

#include <spdlog/cfg/env.h>
#include <spdlog/common.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <tracy/Tracy.hpp>

namespace astri::log {
  /**
   * @brief A spdlog logger sink for sending log messages to tracy
   *
   * This is a specialized logger for integrating [spdlog](https://github.com/gabime/spdlog) with
   * [Tracy](https://github.com/wolfpld/tracy), allowing the inspection and search of log messages
   * to happen directly in tracy along with the other profiling information.
   *
   * @tparam Mutex The mutex type to use for handling multi-threaded logging requests
   */
  template <typename Mutex> class TracySink : public spdlog::sinks::base_sink<Mutex> {
  protected:
    /**
     * @brief Log a message to the sink
     *
     * This formats the log message using the configured formatter, and then sends
     * the message to Tracy using `TracyMessageC`. For visual convinience this
     * method also colorizes the logs that it sends to Tracy based on the log
     * level.
     *
     * @param msg The spdlog message to write to Tracy
     */
    void sink_it_(const spdlog::details::log_msg &msg) override {
      spdlog::memory_buf_t formatted;
      spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
      std::string formatted_string = fmt::to_string(formatted);
      switch (msg.level) {
      case spdlog::level::critical:
        TracyMessageC(formatted_string.c_str(), formatted_string.size(), 0xf38ba8);
        break;
      case spdlog::level::err:
        TracyMessageC(formatted_string.c_str(), formatted_string.size(), 0xfab387);
        break;
      case spdlog::level::warn:
        TracyMessageC(formatted_string.c_str(), formatted_string.size(), 0xf9e2af);
        break;
      case spdlog::level::info:
        TracyMessageC(formatted_string.c_str(), formatted_string.size(), 0xa6e3a1);
        break;
      case spdlog::level::debug:
        TracyMessageC(formatted_string.c_str(), formatted_string.size(), 0x74c7ec);
        break;
      case spdlog::level::trace:
        TracyMessageC(formatted_string.c_str(), formatted_string.size(), 0xcba6f7);
        break;

      default:
        break;
      }
    }

    /**
     * @brief Flush logs buffered in the sink
     *
     * This is a no-op method, that is required to be defined from the spdlog base class. It is not
     * required as the messages are always sent to tracy, there is no need to flushing any internal
     * buffers.
     */
    void flush_() override {}
  };
} // namespace astri::log

using tracy_sink_mt = astri::log::TracySink<std::mutex>;
using tracy_sink_st = astri::log::TracySink<spdlog::details::null_mutex>;

static std::shared_ptr<spdlog::sinks::dist_sink_mt> sink = nullptr;

bool astri::log::initialize() {
  spdlog::sink_ptr csink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  csink->set_pattern("\033[90m[%Y-%m-%d %H:%M:%S.%e]\033[0m \033[1m%^%-8l%$\033[0m  %v");
  csink->set_level(spdlog::level::trace);

  spdlog::sink_ptr fsink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("output.log", true);
  fsink->set_pattern("%Y-%m-%dT%H:%M:%S.%e [%l] %v");
  fsink->set_level(spdlog::level::trace);

  sink = std::make_shared<spdlog::sinks::dist_sink_mt>(std::vector<spdlog::sink_ptr>{csink, fsink});
  sink->set_level(spdlog::level::trace);

#ifdef TRACY_ENABLE
  spdlog::sink_ptr tsink = std::make_shared<tracy_sink_mt>();
  tsink->set_level(spdlog::level::debug);
  tsink->set_pattern("[%l] %v");
  sink->add_sink(tsink);
#endif

  auto logger = std::make_shared<spdlog::logger>("astri", sink);
  spdlog::set_default_logger(logger);

  spdlog::cfg::load_env_levels();

  return true;
}

std::shared_ptr<spdlog::logger> astri::log::get(std::string_view name) {
  std::shared_ptr<spdlog::logger> logger = spdlog::get(name.data());
  if (!logger) {
    LOG_DEBUG("Creating new logger '{}'", name);
    logger = std::make_shared<spdlog ::logger>("astri::" + std::string(name), sink);
    spdlog::register_logger(logger);
  }
  return logger;
}
