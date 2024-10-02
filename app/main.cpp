#include <iostream>

#include <astri/log.hpp>
#include <astri/version.hpp>
#include <cxxopts.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

#if defined(_WIN32) || defined(_WIN64)
#  include <io.h>
#  include <windows.h>
#elif defined(__APPLE__) || defined(__unix__) || defined(__unix)
#  include <unistd.h>
#endif

#ifdef __EMSCRIPTEN__
#  include "web/log_sink.hpp"
#else
#endif

int main(int argc, char const *argv[]) {
#ifdef __EMSCRIPTEN__
  spdlog::sink_ptr console_sink = std::make_shared<app::web::console_sink_mt>();
  console_sink->set_pattern("%v");
  console_sink->set_level(spdlog::level::debug);

  spdlog::set_default_logger(std::make_shared<spdlog::logger>("specula", console_sink));
#else
  try {
    cxxopts::Options options("astri", "");

    // clang-format off
  options.add_options()
    ("h,help", "Print this help message and exit")
    ("V,version", "Print the version information and exit")
  ;

  options.parse_positional({});
    // clang-format on

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
      std::cout << options.help() << std::endl;
      exit(0);
    } else if (result.count("version")) {
      std::cout << ASTRI_VERSION << std::endl;
      exit(0);
    }

#  if defined(_WIN32) || defined(_WIN64)
    const bool use_color = _isatty(_fileno(stdout));
#  elif defined(__APPLE__) || defined(__unix__) || defined(__unix)
    const bool use_color = ::isatty(fileno(stdout)) != 0;
#  else
    const bool use_color = false;
#  endif
    std::shared_ptr<spdlog::sinks::sink> stdout_sink = nullptr;
    if (use_color)
      stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    else
      stdout_sink = std::make_shared<spdlog::sinks::stdout_sink_mt>();

    stdout_sink->set_level(spdlog::level::trace);

    if (use_color)
      stdout_sink->set_pattern("\033[90m[%H:%M:%S.%e]\033[0m \033[1m%^%-8l%$\033[0m  %v");
    else
      stdout_sink->set_pattern("%H:%M:%S.%e %l %v");

    spdlog::set_default_logger(std::make_shared<spdlog::logger>("specula", stdout_sink));

  } catch (const cxxopts::exceptions::specification &e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  } catch (const cxxopts::exceptions::parsing &e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }
#endif

  LOG_INFO("Astri v{} ({})", ASTRI_VERSION, ASTRI_VERSION_COMMIT);

  return 0;
}
