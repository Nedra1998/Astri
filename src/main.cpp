#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <iostream>
#include <spdlog/common.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>

#include "gui/style.hpp"
#include "logging.hpp"
#include "version.hpp"

#if defined(_WIN32) || defined(_WIN64)
#  include <io.h>
#  include <windows.h>
#elif defined(__APPLE__) || defined(__unix__) || defined(__unix)
#  include <unistd.h>
#endif

using namespace astri::gui;

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

  glfwSetErrorCallback(astri::logging::callback);

  LLINFO("OpenGL v{}.{}", 3, 2);
  if (!glfwInit())
    return 1;

  const char *glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow *window = glfwCreateWindow(1280, 720, "Astri", NULL, NULL);
  if (window == nullptr)
    return 1;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO &io = ImGui::GetIO();

  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  Style &style = Style::get();
  style.add_font(Style::REGULAR);
  style.add_font(Style::BOLD);
  style.add_font(Style::ITALIC);
  style.add_font(Style::BOLD_ITALIC);

  style.add_font("H1", Style::BOLD, 32.0f);
  style.add_font("H2", Style::BOLD, 24.0f);
  style.add_font("H3", Style::BOLD, 20.0f);

  // add_window<ChangelogWindow>();
  // add_window<ImGuiDemoWindow>();
  // add_window<ImPlotDemoWindow>();

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    // main_menu();
    // draw_windows();

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
