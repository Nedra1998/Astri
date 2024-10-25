#include "ui/window.hpp"

#include <imgui.h>
#include <implot.h>

void astri::ui::Window::render() {
  if (!open)
    return;
  ImGui::Begin(name.c_str(), &open, flags);
  this->draw();
  ImGui::End();
}

void astri::ui::WindowManager::render() {
  for (auto &window : windows)
    window.second->render();
}

void astri::ui::WindowManager::update() {
  for (auto &window : windows) {
    window.second->update();
  }
}

void astri::ui::WindowManager::prune() {
  std::vector<std::string> to_remove;
  for (auto &window : windows) {
    if (!window.second->open) {
      if (!window.second->close()) {
        window.second->open = true;
        continue;
      }
      to_remove.push_back(window.first);
    }
  }

  if (to_remove.empty())
    return;

  LOG_DEBUG("Window manager pruning {} windows", to_remove.size());

  for (auto &window : to_remove) {
    windows.erase(window);
  }
}

void astri::ui::ImGuiDemoWindow::render() {
  if (!open)
    return;
  ImGui::ShowDemoWindow(&open);
}

void astri::ui::ImPlotDemoWindow::render() {
  if (!open)
    return;
  ImPlot::ShowDemoWindow(&open);
}
