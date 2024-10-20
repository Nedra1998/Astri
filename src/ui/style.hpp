#ifndef INCLUDE_UI_STYLE_HPP_
#define INCLUDE_UI_STYLE_HPP_

#include <string_view>

#include <imgui.h>

namespace astri::ui {
  class Style {
  public:
    static Style &get() {
      static Style instance;
      return instance;
    }

    Style(Style const &) = delete;
    void operator=(Style const &) = delete;

    bool load_fonts(float size = 16.0f);

    ImGuiIO *io = nullptr;
    ImFont *bold = nullptr, *italic = nullptr, *bold_italic = nullptr, *regular = nullptr;

  private:
    Style() { io = &ImGui::GetIO(); }

    ImFont *load_font(std::string_view name, const char *data, float size);
  };
} // namespace astri::ui

#endif // INCLUDE_UI_STYLE_HPP_
