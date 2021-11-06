#pragma once

#include <functional>
#include <string>

namespace astri::gui {
class Window {
public:
  Window(std::string title, bool enabled = false,
         std::function<void()> render = nullptr);

  void operator()();
  virtual void draw();
  virtual void render();

  inline const std::string &title() const { return title_; }
  inline bool enabled() const { return enabled_; }
  inline void enabled(bool setting) { enabled_ = setting; }

protected:
  std::string title_;
  bool enabled_ = false;

  std::function<void()> render_callback_ = nullptr;
};
} // namespace astri::gui
