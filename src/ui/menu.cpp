#include "ui/menu.hpp"

#include <imgui.h>

#include "ecs.hpp"
#include "gl/gl.hpp"
#include "log.hpp"
#include "ui/style.hpp"
#include "ui/window.hpp"
#include "version.hpp"

void astri::ui::main_menu() {
  LOG_ZONE();

  static const ImGuiIO &io = ImGui::GetIO();
  static const Style &style = Style::get();

  static const WindowPtr *win_list = ecs::WORLD.lookup("win_list").get<WindowPtr>();

  static const WindowPtr *win_imgui = ecs::WORLD.lookup("imgui_demo").get<WindowPtr>();
  static const WindowPtr *win_implot = ecs::WORLD.lookup("implot_demo").get<WindowPtr>();

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

    if (win_list != nullptr &&
        ImGui::MenuItem((*win_list)->name.c_str(), nullptr, (*win_list)->open))
      (*win_list)->open = !(*win_list)->open;

    ImGui::Separator();

    if (win_imgui != nullptr && ImGui::MenuItem("  ImGui Demo", nullptr, (*win_imgui)->open))
      (*win_imgui)->open = !(*win_imgui)->open;
    if (win_implot != nullptr && ImGui::MenuItem("  ImPlot Demo", nullptr, (*win_implot)->open))
      (*win_implot)->open = !(*win_implot)->open;

    ImGui::Separator();

    if (ImGui::MenuItem("  Quit Astri"))
      glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);

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
