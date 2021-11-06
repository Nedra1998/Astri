#include <iostream>
#include <spdlog/common.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>

#include "logging.hpp"
#include "version.hpp"

#if defined(_WIN32) || defined(_WIN64)
#  include <io.h>
#  include <windows.h>
#elif defined(__APPLE__) || defined(__unix__) || defined(__unix)
#  include <unistd.h>
#endif

int main(int argc, char *argv[]) {
  try {
    astri::logging::sink = std::make_shared<spdlog::sinks::dist_sink_mt>();
    astri::logging::sink->set_pattern(
        "[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] [%s:%#] <%t> %v");

#ifndef NDEBUG
    {
#  if defined(_WIN32) || defined(_WIN64)
      const bool use_color = _isatty(_fileno(stdout));
#  elif defined(__APPLE__) || defined(__unix__) || defined(__unix)
      const bool use_color = ::isatty(fileno(stdout)) != 0;
#  else
      const bool use_color = false;
#  endif

      std::shared_ptr<spdlog::sinks::sink> sink = nullptr;
      if (use_color)
        sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
      else
        sink = std::make_shared<spdlog::sinks::stdout_sink_mt>();

      sink->set_level(spdlog::level::debug);
      if (use_color)
        sink->set_pattern(
            "[\033[33m%H:%M:%S.%e\033[0m] [\033[1m%n\033[0m] [%^%l%$] "
            "[\033[35m%s\033[0m:\033[36m%#\033[0m:\033[90m%!\033[0m] "
            "<\033[34m%t\033[0m> %v");
      else
        sink->set_pattern("[%H:%M:%S.%e] [%n] [%^%l%$] [%s:%#:%!] <%t> %v");
      astri::logging::sink->add_sink(sink);
    }
#endif

    {
      std::shared_ptr<spdlog::sinks::sink> sink =
          std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
              "logs/astri.log", 5 * 1024 * 1024, 5);
      sink->set_level(spdlog::level::trace);
      sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] [%s:%#] <%t> %v");
      astri::logging::sink->add_sink(sink);
    }

  } catch (const spdlog::spdlog_ex &err) {
    std::cerr << "Log initialization failed: " << err.what() << std::endl;
    return -1;
  }

  LLINFO("Astri v{}", astri::version::semver);

  return 0;
}
