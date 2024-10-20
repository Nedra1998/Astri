#ifndef INCLUDE_SRC_LOG_HPP_
#define INCLUDE_SRC_LOG_HPP_

#include <fmt/ostream.h>
#include <fmt/std.h>
#include <spdlog/spdlog.h>

#ifdef __EMSCRIPTEN__
#  include <emscripten/emscripten.h>
#  include <spdlog/details/null_mutex.h>
#  include <spdlog/sinks/base_sink.h>
#endif

#define LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define LOG_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

#define LOGGER_TRACE(...) SPDLOG_LOGGER_TRACE(__VA_ARGS__)
#define LOGGER_DEBUG(...) SPDLOG_LOGGER_DEBUG(__VA_ARGS__)
#define LOGGER_INFO(...) SPDLOG_LOGGER_INFO(__VA_ARGS__)
#define LOGGER_WARN(...) SPDLOG_LOGGER_WARN(__VA_ARGS__)
#define LOGGER_ERROR(...) SPDLOG_LOGGER_ERROR(__VA_ARGS__)
#define LOGGER_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(__VA_ARGS__)

#define LOG_ZONE() SPDLOG_TRACE("{}", __PRETTY_FUNCTION__)

#ifdef __EMSCRIPTEN__
namespace astri::web {
  template <typename Mutex> class ConsoleSink : public spdlog::sinks::base_sink<Mutex> {
  protected:
    void sink_it_(const spdlog::details::log_msg &msg) override {
      spdlog::memory_buf_t formatted;
      spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
      std::string formatted_string = fmt::to_string(formatted);
      switch (msg.level) {
      case spdlog::level::critical:
        emscripten_log(EM_LOG_NO_PATHS | EM_LOG_CONSOLE | EM_LOG_ERROR, "%s",
                       formatted_string.c_str());
        break;
      case spdlog::level::err:
        emscripten_log(EM_LOG_NO_PATHS | EM_LOG_CONSOLE | EM_LOG_ERROR, "%s",
                       formatted_string.c_str());
        break;
      case spdlog::level::warn:
        emscripten_log(EM_LOG_NO_PATHS | EM_LOG_CONSOLE | EM_LOG_WARN, "%s",
                       formatted_string.c_str());
        break;
      case spdlog::level::info:
        emscripten_log(EM_LOG_NO_PATHS | EM_LOG_CONSOLE | EM_LOG_INFO, "%s",
                       formatted_string.c_str());
        break;
      case spdlog::level::debug:
        emscripten_log(EM_LOG_NO_PATHS | EM_LOG_CONSOLE | EM_LOG_DEBUG, "%s",
                       formatted_string.c_str());
        break;
      case spdlog::level::trace:
        emscripten_log(EM_LOG_NO_PATHS | EM_LOG_CONSOLE | EM_LOG_DEBUG, "%s",
                       formatted_string.c_str());
        break;

      default:
        break;
      }
    }

    void flush_() override {}
  };

  using console_sink_mt = ConsoleSink<std::mutex>;
  using console_sink_st = ConsoleSink<spdlog::details::null_mutex>;
} // namespace astri::web
#endif // __EMSCRIPTEN__

#endif // INCLUDE_SRC_LOG_HPP_
