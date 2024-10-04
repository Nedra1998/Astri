#include "handlers.hpp"

#include <imgui.h>

#include "log.hpp"

#ifdef __EMSCRIPTEN__
#  include <string>

#  include <emscripten.h>
#  include <emscripten/html5.h>
#  include <emscripten_browser_clipboard.h>
#  include <emscripten_browser_cursor.h>

static std::string clipboard_context;

EM_JS(int, canvas_get_width, (), { return Module.canvas.width; });
EM_JS(int, canvas_get_height, (), { return Module.canvas.height; });

inline const char *get_clipboard_for_imgui(ImGuiContext *) { return clipboard_context.c_str(); }
inline void set_clipboard_from_imgui(ImGuiContext *, const char *text) {
  clipboard_context = text;
  emscripten_browser_clipboard::copy(clipboard_context);
}
#endif // __EMSCRIPTEN__

std::pair<std::uint32_t, std::uint32_t> app::ResizeHandler::initial() {
  LOG_ZONE();
#ifdef __EMSCRIPTEN__
  return {canvas_get_width(), canvas_get_height()};
#else
  return {680, 480};
#endif // __EMSCRIPTEN__
}

void app::ResizeHandler::update(GLFWwindow *window) {
  LOG_ZONE();
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

bool app::ClipboardHandler::configure() {
  LOG_ZONE();
#ifdef __EMSCRIPTEN__
  emscripten_browser_clipboard::paste(
      [](std::string const &paste_data, void *callback_data [[maybe_unused]]) {
        clipboard_context = std::move(paste_data);
      });

  ImGuiPlatformIO &platform_io = ImGui::GetPlatformIO();
  platform_io.Platform_GetClipboardTextFn = get_clipboard_for_imgui;
  platform_io.Platform_SetClipboardTextFn = set_clipboard_from_imgui;
#endif
  return true;
}

app::CursorHandler::CursorHandler() {
  LOG_ZONE();
#ifdef __EMSCRIPTEN__
  io = &ImGui::GetIO();

  io->ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  io->Fonts->Flags |= ImFontAtlasFlags_NoMouseCursors;
#endif
}

void app::CursorHandler::render() {
#ifdef __EMSCRIPTEN__
  LOG_ZONE();
  if (io->WantCaptureMouse) {
    switch (ImGui::GetMouseCursor()) {
    case ImGuiMouseCursor_Arrow:
      emscripten_browser_cursor::set(emscripten_browser_cursor::cursor::cursor_default);
      break;
    case ImGuiMouseCursor_TextInput:
      emscripten_browser_cursor::set(emscripten_browser_cursor::cursor::text);
      break;
    case ImGuiMouseCursor_ResizeAll:
      emscripten_browser_cursor::set(emscripten_browser_cursor::cursor::move);
      break;
    case ImGuiMouseCursor_ResizeNS:
      emscripten_browser_cursor::set(emscripten_browser_cursor::cursor::ns_resize);
      break;
    case ImGuiMouseCursor_ResizeEW:
      emscripten_browser_cursor::set(emscripten_browser_cursor::cursor::ew_resize);
      break;
    case ImGuiMouseCursor_ResizeNESW:
      emscripten_browser_cursor::set(emscripten_browser_cursor::cursor::nesw_resize);
      break;
    case ImGuiMouseCursor_ResizeNWSE:
      emscripten_browser_cursor::set(emscripten_browser_cursor::cursor::nwse_resize);
      break;
    case ImGuiMouseCursor_Hand:
      emscripten_browser_cursor::set(emscripten_browser_cursor::cursor::pointer);
      break;
    case ImGuiMouseCursor_NotAllowed:
      emscripten_browser_cursor::set(emscripten_browser_cursor::cursor::not_allowed);
      break;
    }
  }
#endif
}
