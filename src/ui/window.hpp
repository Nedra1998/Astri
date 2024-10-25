#ifndef INCLUDE_UI_WINDOW_HPP_
#define INCLUDE_UI_WINDOW_HPP_

#include <memory>
#include <string>
#include <unordered_map>

#include "log.hpp"

namespace astri::ui {
  class Window {
  public:
    Window(std::string name, uint32_t flags = 0, bool open = true)
        : name(name), flags(flags), open(open) {}

    std::string name;
    bool open = false;
    uint32_t flags = 0;

    virtual void render();

    virtual void draw() {};
    virtual void update() {};
    virtual bool close() { return true; };
  };

  class WindowManager {
  public:
    static WindowManager &get() {
      static WindowManager instance;
      return instance;
    }

    void render();
    void update();
    void prune();

    inline bool has(std::string name) { return windows.find(name) != windows.end(); }
    std::shared_ptr<Window> get(std::string name) {
      auto win = windows.find(name);
      if (win == windows.end())
        return nullptr;
      return win->second;
    }

    template <typename T, typename... Args>
    std::shared_ptr<T> open(std::string name, Args... args) {
      auto ptr = std::make_shared<T>(name, args...);
      windows[name] = std::dynamic_pointer_cast<Window>(ptr);
      LOG_DEBUG("Window manager handling {} windows", windows.size());
      return ptr;
    }

    template <typename T, typename... Args>
    std::shared_ptr<T> toggle(std::string name, Args... args) {
      auto win = windows.find(name);
      if (win != windows.end()) {
        if (!win->second->close())
          return std::dynamic_pointer_cast<T>(win->second);
        windows.erase(win);
        return nullptr;
      }
      return open<T>(name, args...);
    }

    std::unordered_map<std::string, std::shared_ptr<Window>> windows;

  private:
    WindowManager() = default;
    ~WindowManager() = default;
    WindowManager(const WindowManager &) = delete;
    WindowManager &operator=(const WindowManager &) = delete;
  };

  class ImGuiDemoWindow : public Window {
  public:
    ImGuiDemoWindow(std::string name) : Window(name) {}

    void render() override;
  };

  class ImPlotDemoWindow : public Window {
  public:
    ImPlotDemoWindow(std::string name) : Window(name) {}

    void render() override;
  };
} // namespace astri::ui

#endif // INCLUDE_UI_WINDOW_HPP_
