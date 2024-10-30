#ifndef INCLUDE_UI_WINDOW_HPP_
#define INCLUDE_UI_WINDOW_HPP_

#include <cstdint>
#include <memory>
#include <string>

namespace astri::ui {
  class Window {
  public:
    Window(std::string name, uint32_t flags = 0, bool open = false)
        : name(name), flags(flags), open(open) {}

    std::string name;
    bool open = false;
    uint32_t flags = 0;

    virtual void render();

    virtual void draw() {};
    virtual void update() {};
    virtual bool close() { return true; };
  };

  typedef std::shared_ptr<Window> WindowPtr;

  class ImGuiDemoWindow : public Window {
  public:
    ImGuiDemoWindow(std::string name, bool open = false) : Window(name, 0, open) {}

    void render() override;
  };

  class ImPlotDemoWindow : public Window {
  public:
    ImPlotDemoWindow(std::string name, bool open = false) : Window(name, 0, open) {}

    void render() override;
  };
} // namespace astri::ui

#endif // INCLUDE_UI_WINDOW_HPP_
