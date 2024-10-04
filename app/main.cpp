#include <functional>
#include <iostream>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <astri/log.hpp>
#include <astri/version.hpp>
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

#include "glad/glad.h"
#include "handlers.hpp"

#ifdef __EMSCRIPTEN__
#  include "web/log_sink.hpp"
#else
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

void glfw_callback(int error, const char *description) {
  LOG_ERROR("GLFW Error {}: {}", error, description);
}

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

  const auto [width, height] = app::ResizeHandler::initial();

  glfwSetErrorCallback(glfw_callback);
  if (!glfwInit()) {
    LOG_CRITICAL("Failed to initialize GLFW");
    return -1;
  }

  // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char *glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  LOG_INFO("OpenGL ES v{} + GLSL {}", "2.0", 100);
#elif defined(__APPLE__)
  // GL 3.2 + GLSL 150
  const char *glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
  LOG_INFO("OpenGL v{} + GLSL {}", "3.2", 150);
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
  LOG_INFO("OpenGL v{} + GLSL {}", "3.0", 130);
#endif

  GLFWwindow *window = glfwCreateWindow(width, height, "Astri", NULL, NULL);
  if (!window) {
    LOG_CRITICAL("Failed to create GLFW window");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = NULL;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImPlotStyle &implot_style = ImPlot::GetStyle();
  implot_style.UseLocalTime = true;
  implot_style.UseISO8601 = true;

  ImGui::StyleColorsDark();
  ImPlot::StyleColorsDark();

#ifdef __EMSCRIPTEN__
  if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
#else
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
#endif
    LOG_CRITICAL("Failed to initialize GLAD");
    glfwTerminate();
    return 1;
  }

  ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
  ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
  ImGui_ImplOpenGL3_Init(glsl_version);

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
  glGenVertexArraysOES(1, &VAO);
  glBindVertexArrayOES(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  const char *vertexShaderSource = "#version 100\n"
                                   "#ifdef GL_ES\n"
                                   "precision mediump float;\n"
                                   "#endif\n"
                                   "attribute vec3 aPos;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                   "}\0";
  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    LOG_ERROR("Vertex shader compilation failed: {}", infoLog);
  }

  const char *fragmentShaderSource = "#version 100\n"
                                     "#ifdef GL_ES\n"
                                     "precision mediump float;\n"
                                     "#endif\n"
                                     "void main()\n"
                                     "{\n"
                                     "    gl_FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n"
                                     "}";

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    LOG_ERROR("Fragment shader compilation failed: {}", infoLog);
  }

  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    LOG_ERROR("Shader program linking failed: {}", infoLog);
  }

  glUseProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

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

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArrayOES(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow *backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(window);
  }
#ifdef __EMSCRIPTEN__
  EMSCRIPTEN_MAINLOOP_END;
#endif

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
