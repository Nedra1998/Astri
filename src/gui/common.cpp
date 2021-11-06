#include "gui/common.hpp"
#include "gui/window.hpp"

#include <imgui.h>
#include <implot.h>

#include "changelog.hpp"
#include "gui/style.hpp"
#include "logging.hpp"
#include "version.hpp"

namespace astri::gui {
std::vector<std::shared_ptr<Window>> windows_;
}

void astri::gui::MarkdownWindow::render() { Style::get().markdown(text_); }

astri::gui::ChangelogWindow::ChangelogWindow() : MarkdownWindow("Changelog") {
  text_.resize(changelog_size);
  for (std::size_t i = 0; i < changelog_size; i += 4) {
    const unsigned int blk = changelog_data[i / 4];
    text_[i]               = (blk & 0xff);
    text_[i + 1]           = ((blk >> 8) & 0xff);
    text_[i + 2]           = ((blk >> 16) & 0xff);
    text_[i + 3]           = ((blk >> 24) & 0xff);
  }
}

void astri::gui::ImGuiDemoWindow::draw() {
  if (enabled_) {
    LTRACE("imgui", "Rendering ImGui window \"{}\"", title_);
    ImGui::ShowDemoWindow(&enabled_);
  }
}
void astri::gui::ImPlotDemoWindow::draw() {
  if (enabled_) {
    LTRACE("imgui", "Rendering ImGui window \"{}\"", title_);
    ImPlot::ShowDemoWindow(&enabled_);
  }
}

void astri::gui::main_menu() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::MenuItem("Astri"))
      ImGui::OpenPopup("Astri Information");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    information_popup();

    if (ImGui::BeginMenu("Windows")) {
      for (const auto &win : windows_) {
        if (ImGui::MenuItem(win->title().c_str(), NULL, win->enabled())) {
          win->enabled(!win->enabled());
        }
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void astri::gui::information_popup() {
  bool _info_modal = true;
  if (ImGui::BeginPopupModal("Astri Information", &_info_modal,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    Style &style = Style::get();
    ImFont *bold = style.bold(), *italic = style.italic();

    if (bold != nullptr)
      ImGui::PushFont(bold);
    ImGui::Text("          Version: ");
    if (bold != nullptr)
      ImGui::PopFont();

    ImGui::SameLine();
    ImGui::Text("%s", astri::version::semver.c_str());

    if (bold != nullptr)
      ImGui::PushFont(bold);
    ImGui::Text("         Compiler: ");
    if (bold != nullptr)
      ImGui::PopFont();

    ImGui::SameLine();
#if defined(__clang__) && defined(__apple_build_version)
    ImGui::Text("%s v%d.%d.%d", "AppleClang", __clang_major__, __clang_minor__,
                __clang_patchlevel__);
#elif defined(__clang__)
    ImGui::Text("%s v%d.%d.%d", "Clang", __clang_major__, __clang_minor__,
                __clang_patchlevel__);
#elif defined(__GNUC__)
#  if defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
    ImGui::Text("%s v%d.%d.%d", "GCC", __GNUC__, __GNUC_MINOR__,
                __GNUC_PATCHLEVEL__);
#  elif defined(__GNUC_MINOR__)
    ImGui::Text("%s v%d.%d", "GCC", __GNUC__, __GNUC_MINOR__);
#  else
    ImGui::Text("%s v%d", "GCC", __GNUC__);
#  endif
#elif defined(_MSC_VER)
#  if defined(_MSC_FULL_VER)
#    if _MSC_VER >= 1400
    ImGui::Text("%s v%d.%d.%d", "MSVC", _MSC_VER / 100, _MSC_VER % 100,
                _MSC_FULL_VER % 100000);
#    else
    ImGui::Text("%s v%d.%d.%d", "MSVC", _MSC_VER / 100, _MSC_VER % 100,
                _MSC_FULL_VER % 10000);
#    endif
#  else
    ImGui::Text("%s v%d.%d", "MSVC", _MSC_VER / 100, _MSC_VER % 100);
#  endif
#else
    ImGui::Text("UNKNOWN");
#endif

    if (bold != nullptr)
      ImGui::PushFont(bold);
    ImGui::Text("Compiled DateTime: ");
    if (bold != nullptr)
      ImGui::PopFont();

    ImGui::SameLine();
    char d[3], m[3];
    int year, mon, mday, hour, min, sec;
#if defined(__TIMESTAMP__)
    std::sscanf(__TIMESTAMP__, "%s %s %d %d:%d:%d %d", d, m, &mday, &hour, &min,
                &sec, &year);
#else
    std::sscanf(__DATE__, "%s %d %d", m, &mday, &year);
    std::sscanf(__TIME__, "%d:%d:%d", &hour, &min, &sec);
#endif
    if (!std::strncmp(m, "Jan", 3))
      mon = 1;
    else if (!std::strncmp(m, "Feb", 3))
      mon = 2;
    else if (!std::strncmp(m, "Mar", 3))
      mon = 3;
    else if (!std::strncmp(m, "Apr", 3))
      mon = 4;
    else if (!std::strncmp(m, "May", 3))
      mon = 5;
    else if (!std::strncmp(m, "Jun", 3))
      mon = 6;
    else if (!std::strncmp(m, "Jul", 3))
      mon = 7;
    else if (!std::strncmp(m, "Aug", 3))
      mon = 8;
    else if (!std::strncmp(m, "Sep", 3))
      mon = 9;
    else if (!std::strncmp(m, "Oct", 3))
      mon = 10;
    else if (!std::strncmp(m, "Nov", 3))
      mon = 11;
    else if (!std::strncmp(m, "Dec", 3))
      mon = 12;
    ImGui::Text("%04d-%02d-%02dT%02d:%02d:%02d", year, mon, mday, hour, min,
                sec);

    ImGui::EndPopup();
  }
}
