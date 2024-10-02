#ifndef INCLUDE_WEB_LOG_SINK_HPP_
#define INCLUDE_WEB_LOG_SINK_HPP_

#ifdef __EMSCRIPTEN__
#  include <emscripten/emscripten.h>
#  include <spdlog/details/null_mutex.h>
#  include <spdlog/sinks/base_sink.h>
#  include <spdlog/spdlog.h>

namespace app::web {
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

} // namespace app::web

#endif

#endif // INCLUDE_WEB_LOG_SINK_HPP_
