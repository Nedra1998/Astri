#include "ui/ui.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

#include "log.hpp"
#include "ui/style.hpp"
#include "ui/window.hpp"
#include "version.hpp"

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

  ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

  main_menu();

  WindowManager &wm = ui::WindowManager::get();

  wm.render();
  wm.prune();

  // ImGui::ShowDemoWindow();
  // ImPlot::ShowDemoWindow();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void astri::ui::main_menu() {
  LOG_ZONE();
  const Style &style = Style::get();
  const ImGuiIO &io = ImGui::GetIO();
  WindowManager &wm = WindowManager::get();

  ImGui::BeginMainMenuBar();

  ImGui::PushFont(style.bold);
  if (ImGui::BeginMenu("  Astri")) {
    ImGui::PushFont(style.regular);

    ImGui::BeginDisabled();
#ifdef NDEBUG
    ImGui::Text("Astri v%s (%s)", ASTRI_VERSION, ASTRI_VERSION_COMMIT);
#else
    ImGui::Text("  Astri v%s (%s) - %s", ASTRI_VERSION, ASTRI_VERSION_COMMIT, ASTRI_BUILD_TYPE);
#endif // NDEBUG
    ImGui::EndDisabled();

    ImGui::Separator();

    if (ImGui::MenuItem("  ImGui Demo", nullptr, wm.has("  ImGui Demo")))
      wm.toggle<ImGuiDemoWindow>("  ImGui Demo");

    if (ImGui::MenuItem("  ImPlot Demo", nullptr, wm.has("  ImPlot Demo")))
      wm.toggle<ImPlotDemoWindow>("  ImPlot Demo");

    ImGui::PopFont();
    ImGui::EndMenu();
  }
  ImGui::PopFont();

  std::string fps = fmt::format("{:.1f} FPS ({:.1f} ms)", io.Framerate, 1000.0f / io.Framerate);
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x -
                       ImGui::CalcTextSize(fps.c_str()).x);
  ImGui::BeginDisabled();
  ImGui::Text("%s", fps.c_str());
  ImGui::EndDisabled();

  ImGui::EndMainMenuBar();
}
