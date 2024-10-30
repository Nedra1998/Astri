#include "ui/ui.hpp"

#include <memory>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

#include "ecs.hpp"
#include "log.hpp"
#include "ui/menu.hpp"
#include "ui/style.hpp"
#include "ui/window.hpp"
#include "ui/windows/list.hpp"

static flecs::system window_update, window_draw;

bool astri::ui::initialize(GLFWwindow *window) {
  LOG_ZONE();

  LOG_INFO("ImGui v{}", IMGUI_VERSION);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = NULL;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);

  ImGui_ImplOpenGL3_Init(gl::GLSL_VERSION.c_str());

  LOG_INFO("ImPlot v{}", IMPLOT_VERSION);
  ImPlot::CreateContext();

  ImPlotStyle &implot_style = ImPlot::GetStyle();
  implot_style.UseLocalTime = true;
  implot_style.UseISO8601 = true;

  ImPlot::StyleColorsDark();

  if (!Style::get().load_fonts())
    return false;

  window_update =
      ecs::WORLD.system<WindowPtr>("WindowUpdate").each([](WindowPtr &ptr) { ptr->update(); });
  window_draw =
      ecs::WORLD.system<WindowPtr>("WindowRender").each([](WindowPtr &ptr) { ptr->render(); });

  ecs::WORLD.entity("win_list")
      .set(std::dynamic_pointer_cast<Window>(std::make_shared<ListWindow>()));

  // clang-format off
  ecs::WORLD.entity("imgui_demo")
      .set(std::dynamic_pointer_cast<Window>(std::make_shared<ImGuiDemoWindow>("  ImGui Demo")));
  ecs::WORLD.entity("implot_demo")
      .set(std::dynamic_pointer_cast<Window>(std::make_shared<ImPlotDemoWindow>("  ImPlot Demo")));
  // clang-format on

  return true;
}

void astri::ui::terminate() {
  LOG_ZONE();

  ImPlot::DestroyContext();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();

  ImGui::DestroyContext();
}

void astri::ui::frame(GLFWwindow *window) {
  LOG_ZONE();

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  {
    LOG_ZONEN("void astri::ui::window::update()");
    window_update.run();
  }

  ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

  main_menu();

  {
    LOG_ZONEN("void astri::ui::window::render()");
    window_draw.run();
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
