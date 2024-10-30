#include "ui/window.hpp"

#include <imgui.h>
#include <implot.h>

#include "log.hpp"

void astri::ui::Window::render() {
  if (!open)
    return;
  ImGui::Begin(name.c_str(), &open, flags);
  this->draw();
  ImGui::End();
}

void astri::ui::ImGuiDemoWindow::render() {
  if (!open)
    return;
  LOG_ZONE();
  ImGui::ShowDemoWindow(&open);
}

void astri::ui::ImPlotDemoWindow::render() {
  if (!open)
    return;
  LOG_ZONE();
  ImPlot::ShowDemoWindow(&open);
}
