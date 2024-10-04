#ifndef INCLUDE_APP_HANDLERS_HPP_
#define INCLUDE_APP_HANDLERS_HPP_

#include <cstdint>
#include <tuple>
#include <utility>

#include <imgui.h>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>

namespace app {
  class ResizeHandler {
  public:
    ResizeHandler() { std::tie(_width, _height) = initial(); }
    static std::pair<std::uint32_t, std::uint32_t> initial();
    void update(GLFWwindow *window);

  private:
    std::uint32_t _width, _height;
  };

  class ClipboardHandler {
  public:
    static bool configure();
  };

  class CursorHandler {
  public:
    CursorHandler();
    static bool configure();
    void render();

  private:
    ImGuiIO *io = nullptr;
  };
} // namespace app

#endif // INCLUDE_APP_HANDLERS_HPP_
