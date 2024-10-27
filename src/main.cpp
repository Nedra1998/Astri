#include <spdlog/cfg/env.h>
#include <spdlog/common.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>

#include "core/client.hpp"
#include "core/model.hpp"
#include "gl/gl.hpp"
#include "log.hpp"
#include "ui/ui.hpp"
#include "version.hpp"

#if defined(_WIN32) || defined(_WIN64)
#  include <io.h>
#  include <windows.h>
#elif defined(__APPLE__) || defined(__unix__) || defined(__unix)
#  include <unistd.h>
#endif

int main(int argc, char *argv[]) {

#if defined(_WIN32) || defined(_WIN64)
  const bool use_color = _isatty(_fileno(stdout));
#elif defined(__APPLE__) || defined(__unix__) || defined(__unix)
  const bool use_color = ::isatty(fileno(stdout)) != 0;
#else
  const bool use_color = false;
#endif

  spdlog::sink_ptr csink = nullptr;
  if (use_color) {
    csink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    csink->set_pattern("\033[90m[%H:%M:%S.%e]\033[0m \033[1m%^%-8l%$\033[0m  %v");
  } else {
    csink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
    csink->set_pattern("%H:%M:%S.%e %l %v");
  }
  spdlog::sink_ptr fsink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("output.log", true);
  fsink->set_pattern("%Y-%m-%dT%H:%M:%S.%e [%l] %v");
  spdlog::sink_ptr sink =
      std::make_shared<spdlog::sinks::dist_sink_mt>(std::vector<spdlog::sink_ptr>{csink, fsink});
  csink->set_level(spdlog::level::trace);
  fsink->set_level(spdlog::level::trace);
  sink->set_level(spdlog::level::trace);

  auto logger = std::make_shared<spdlog::logger>("astri", sink);
  spdlog::set_default_logger(logger);

  spdlog::cfg::load_env_levels();

  LOG_INFO("Astri v{} ({})", ASTRI_VERSION, ASTRI_VERSION_COMMIT);

  astri::core::Client client("./_cache");
  // astri::core::Datastore &datastore = astri::core::Datastore::get();
  //
  // if (const auto satellites = client.group(client.GROUPS[0])) {
  //   datastore.insert(client.GROUPS[0], satellites.value());
  // }

  GLFWwindow *window = astri::gl::initialize(1280, 720);
  if (!window) {
    return -1;
  }

  if (!astri::ui::initialize(window)) {
    astri::gl::terminate(window);
    return -1;
  }

  astri::gl::startup(window);

  glClearColor(0.2, 0.2, 0.2, 1.0);
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    astri::gl::frame(window);
    astri::ui::frame(window);

    glfwSwapBuffers(window);
    MARK_FRAME();
  }

  astri::ui::terminate();
  astri::gl::terminate(window);

  return 0;
}
