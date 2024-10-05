#include "gl.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

#include "log.hpp"

void glfw_callback(int error, const char *description) {
  LOG_ERROR("GLFW Error {}: {}", error, description);
}

GLFWwindow *astri::gl::initialize(std::uint32_t width, std::uint32_t height) {
  LOG_ZONE();

  if (!glfwInit()) {
    LOG_CRITICAL("Failed to initialize GLFW");
    return nullptr;
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

  GLFWwindow *window = glfwCreateWindow(width, height, "Astri", nullptr, nullptr);
  if (!window) {
    LOG_CRITICAL("Failed to create GLFW window");
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(window);

#ifndef __EMSCRIPTEN__
  glfwSwapInterval(1); // Enable vsync

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LOG_CRITICAL("GLAD failed to load OpenGL");
    glfwDestroyWindow(window);
    glfwTerminate();
    return nullptr;
  }
#else
  if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
    LOG_CRITICAL("GLAD failed to load OpenGL ES");
    glfwDestroyWindow(window);
    glfwTerminate();
    return nullptr;
  }
#endif

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

  ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
  ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif

  ImGui_ImplOpenGL3_Init(glsl_version);

  return window;
}

void astri::gl::terminate(GLFWwindow *window) {
  LOG_ZONE();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();

  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  if (window)
    glfwDestroyWindow(window);
  glfwTerminate();
}
