#pragma once

#include "gui/window.hpp"
#include <memory>
#include <vector>

namespace astri::gui {
extern std::vector<std::shared_ptr<Window>> windows_;

class MarkdownWindow : public Window {
public:
  MarkdownWindow(std::string title, std::string text = std::string{},
                 bool enabled = false)
      : Window(title, enabled), text_(std::move(text)) {}

  void render() override;

protected:
  std::string text_;
};

// class ChangelogWindow : public MarkdownWindow {
// public:
//   ChangelogWindow();

// private:
//   std::string changelog_;
// };

class ImGuiDemoWindow : public Window {
public:
  ImGuiDemoWindow() : Window("ImGuiDemoWindow") {}

  void draw() override;
};
class ImPlotDemoWindow : public Window {
public:
  ImPlotDemoWindow() : Window("ImPlotDemoWindow") {}

  void draw() override;
};

template <typename T, typename... Args> inline void add_window(Args... args) {
  windows_.push_back(std::move(
      std::dynamic_pointer_cast<Window>(std::make_shared<T>(args...))));
}
template <typename T>
inline typename std::enable_if<std::is_base_of<Window, T>::value, void>::type
add_window(std::unique_ptr<T> win) {
  windows_.push_back(std::move(std::dynamic_pointer_cast<Window>(win)));
}

inline void draw_windows() {
  for (const auto &it : windows_)
    it->draw();
}

void main_menu();
void information_popup();

} // namespace astri::gui
