#include "gui/window.hpp"

#include <functional>
#include <imgui.h>
#include <string>

#include "logging.hpp"

astri::gui::Window::Window(std::string title, bool enabled,
                           std::function<void()> render)
    : title_(std::move(title)), enabled_(enabled),
      render_callback_(std::move(render)) {}

void astri::gui::Window::operator()() { this->draw(); }

void astri::gui::Window::draw() {
  if (enabled_) {
    LTRACE("imgui", "Rendering ImGui window \"{}\"", title_);
    ImGui::Begin(title_.c_str(), &enabled_);
    this->render();
    ImGui::End();
  }
}

void astri::gui::Window::render() {
  if (render_callback_ != nullptr)
    render_callback_();
}
