#include "ui/ui.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

#include "log.hpp"
#include "ui/style.hpp"
#include "ui/window.hpp"
#include "version.hpp"

#ifdef __EMSCRIPTEN__
#  include <emscripten_browser_cursor.h>
#endif

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
#ifdef __EMSCRIPTEN__
  ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif

  ImGui_ImplOpenGL3_Init(gl::GLSL_VERSION.c_str());

  LOG_INFO("ImPlot v{}", IMPLOT_VERSION);
  ImPlot::CreateContext();

  ImPlotStyle &implot_style = ImPlot::GetStyle();
  implot_style.UseLocalTime = true;
  implot_style.UseISO8601 = true;

  ImPlot::StyleColorsDark();

  if (!Style::get().load_fonts())
    return false;

#ifdef __EMSCRIPTEN__
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  io.Fonts->Flags |= ImFontAtlasFlags_NoMouseCursors;
#endif // __EMSCRIPTEN__

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

#ifdef __EMSCRIPTEN__
  set_cursor();
#endif // __EMSCRIPTEN__

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

#ifdef __EMSCRIPTEN__

static emscripten_browser_cursor::cursor last_cursor =
    emscripten_browser_cursor::cursor::cursor_default;

void astri::ui::set_cursor() {
  LOG_ZONE();
  if (ImGui::GetIO().WantCaptureMouse) {
    emscripten_browser_cursor::cursor new_cursor =
        emscripten_browser_cursor::cursor::cursor_default;
    switch (ImGui::GetMouseCursor()) {
    case ImGuiMouseCursor_Arrow:
      new_cursor = emscripten_browser_cursor::cursor::cursor_default;
      break;
    case ImGuiMouseCursor_TextInput:
      new_cursor = emscripten_browser_cursor::cursor::text;
      break;
    case ImGuiMouseCursor_ResizeAll:
      new_cursor = emscripten_browser_cursor::cursor::move;
      break;
    case ImGuiMouseCursor_ResizeNS:
      new_cursor = emscripten_browser_cursor::cursor::ns_resize;
      break;
    case ImGuiMouseCursor_ResizeEW:
      new_cursor = emscripten_browser_cursor::cursor::ew_resize;
      break;
    case ImGuiMouseCursor_ResizeNESW:
      new_cursor = emscripten_browser_cursor::cursor::nesw_resize;
      break;
    case ImGuiMouseCursor_ResizeNWSE:
      new_cursor = emscripten_browser_cursor::cursor::nwse_resize;
      break;
    case ImGuiMouseCursor_Hand:
      new_cursor = emscripten_browser_cursor::cursor::pointer;
      break;
    case ImGuiMouseCursor_NotAllowed:
      new_cursor = emscripten_browser_cursor::cursor::not_allowed;
      break;
    }

    if (new_cursor != last_cursor) {
      emscripten_browser_cursor::set(new_cursor);
      last_cursor = new_cursor;
    }
  }
}
#endif
