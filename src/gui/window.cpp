#include "gui/window.hpp"

#include <functional>
#include <imgui.h>
#include <string>

#include "logging.hpp"

astri::gui::Window::Window(std::string title, bool enabled)
    : title_(std::move(title)), enabled_(enabled) {}

void astri::gui::Window::operator()() { this->draw(); }

void astri::gui::Window::draw() {
  this->update();
  if (enabled_) {
    LTRACE("imgui", "Rendering ImGui window \"{}\"", title_);
    ImGui::Begin(title_.c_str(), &enabled_);
    this->render();
    ImGui::End();
  }
}
