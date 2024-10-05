#include <iostream>

#include <cxxopts.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

#ifdef __EMSCRIPTEN__
#  include <emscripten.h>
#endif // __EMSCRIPTEN__

#if defined(_WIN32) || defined(_WIN64)
#  include <io.h>
#  include <windows.h>
#elif defined(__APPLE__) || defined(__unix__) || defined(__unix)
#  include <unistd.h>
#endif

#include "gl/gl.hpp"
#include "gl/shader.hpp"
#include "handlers.hpp"
#include "log.hpp"
#include "ui/style.hpp"
#include "version.hpp"

#ifdef __EMSCRIPTEN__
#  include "web/log_sink.hpp"
#else
#  include "fragment.h"
#  include "vertex.h"
#endif

// clang-format off
#ifdef __EMSCRIPTEN__
static std::function<void()>            MainLoopForEmscriptenP;
static void MainLoopForEmscripten()     { MainLoopForEmscriptenP(); }
#  define EMSCRIPTEN_MAINLOOP_BEGIN       MainLoopForEmscriptenP = [&]() { do
#  define EMSCRIPTEN_MAINLOOP_END         while (0); }; emscripten_set_main_loop(MainLoopForEmscripten, 0, true)
#else
#  define EMSCRIPTEN_MAINLOOP_BEGIN
#  define EMSCRIPTEN_MAINLOOP_END
#endif
// clang-format on

int main(int argc, char const *argv[]) {
#ifdef __EMSCRIPTEN__
  spdlog::sink_ptr console_sink = std::make_shared<app::web::console_sink_mt>();
  console_sink->set_pattern("%v");
  console_sink->set_level(spdlog::level::debug);

  auto logger = std::make_shared<spdlog::logger>("specula", console_sink);
  logger->set_level(spdlog::level::trace);
  spdlog::set_default_logger(logger);
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

    auto logger = std::make_shared<spdlog::logger>("specula", stdout_sink);
    logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(logger);

  } catch (const cxxopts::exceptions::specification &e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  } catch (const cxxopts::exceptions::parsing &e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }
#endif

  LOG_INFO("Astri v{} ({})", ASTRI_VERSION, ASTRI_VERSION_COMMIT);
  LOG_DEBUG("Initializing GLFW");
  LOG_TRACE("Initializing ImGui");

  const auto [width, height] = app::ResizeHandler::initial();

  GLFWwindow *window = astri::gl::initialize(width, height);
  if (!window)
    return -1;

  astri::ui::Style::get().load_fonts(16.0f);
  app::ClipboardHandler::configure();
  app::CursorHandler cursor_handler;

  static bool imgui_demo_window = true;
  static bool implot_demo_window = true;

  float vertices[] = {
      0.5f,  0.5f,  0.0f, // top right
      0.5f,  -0.5f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f  // top left
  };
  unsigned int indices[] = {
      // note that we start from 0!
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };
  unsigned int VBO;
  glGenBuffers(1, &VBO);

  unsigned int EBO;
  glGenBuffers(1, &EBO);

  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

#ifdef __EMSCRIPTEN__
  astri::Shader shader(std::filesystem::path("assets/shaders/vertex.glsl"),
                       std::filesystem::path("assets/shaders/fragment.glsl"));
#else
  astri::Shader shader(std::string(reinterpret_cast<const char *>(vertex_data), vertex_size),
                       std::string(reinterpret_cast<const char *>(fragment_data), fragment_size));
#endif
  if (!shader.valid()) {
    LOG_ERROR("Shader failed to compile");
    return -1;
  }

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glClearColor(0.0, 0, 0, 1);
#ifdef __EMSCRIPTEN__
  EMSCRIPTEN_MAINLOOP_BEGIN
#else
  while (!glfwWindowShouldClose(window))
#endif
  {
    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(),
                                 ImGuiDockNodeFlags_PassthruCentralNode);

    if (imgui_demo_window)
      ImGui::ShowDemoWindow(&imgui_demo_window);
    if (implot_demo_window)
      ImPlot::ShowDemoWindow(&implot_demo_window);

    cursor_handler.render();

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.use();
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }
#ifdef __EMSCRIPTEN__
  EMSCRIPTEN_MAINLOOP_END;
#endif

  astri::gl::terminate(window);

  return 0;
}
