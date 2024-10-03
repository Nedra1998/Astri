#include "handlers.hpp"

#include <imgui.h>

#ifdef __EMSCRIPTEN__
#  include <emscripten.h>
#  include <emscripten/html5.h>

EM_JS(int, canvas_get_width, (), { return Module.canvas.width; });
EM_JS(int, canvas_get_height, (), { return Module.canvas.height; });
#endif // __EMSCRIPTEN__

std::pair<std::uint32_t, std::uint32_t> app::ResizeHandler::initial() {
#ifdef __EMSCRIPTEN__
  return {canvas_get_width(), canvas_get_height()};
#else
  return {680, 480};
#endif // __EMSCRIPTEN__
}

void app::ResizeHandler::update(GLFWwindow *window) {
#ifdef __EMSCRIPTEN__
  int width = canvas_get_width();
  int height = canvas_get_height();
  if (width != _width || height != _height) {
    _width = width;
    _height = height;
    glfwSetWindowSize(window, _width, _height);
    ImGui::SetCurrentContext(ImGui::GetCurrentContext());
  }
#endif // __EMSCRIPTEN__
}
