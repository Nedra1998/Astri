#pragma once

#include <functional>
#include <string>

namespace astri::gui {
class Window {
public:
  Window(std::string title, bool enabled = false);

  void operator()();
  virtual void draw();
  virtual void render() {}
  virtual void update() {}

  inline const std::string &title() const { return title_; }
  inline bool enabled() const { return enabled_; }
  inline void enabled(bool setting) { enabled_ = setting; }

protected:
  std::string title_;
  bool enabled_ = false;
};
} // namespace astri::gui
