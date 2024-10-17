#include "ui/style.hpp"

#include <map>
#include <string_view>

#include <imgui.h>
#include <implot.h>

#include "log.hpp"

#ifndef __EMSCRIPTEN__
#  include "catppuccin_frappe.h"
#  include "iosevkanerdfont_bold.h"
#  include "iosevkanerdfont_bolditalic.h"
#  include "iosevkanerdfont_italic.h"
#  include "iosevkanerdfont_regular.h"
#endif

static const ImWchar NERD_FONT_GLYPH_RANGES[] = {0x0020, 0x00FF, // Basic Latin + Latin Supplement
                                                 0x23fb, 0x23fe, // IEC Power Symbols
                                                 0x2665, 0x2665, // Octicons
                                                 0x26a1, 0x26a1, // Octicons
                                                 0x2b58, 0x2b58, // IEC Power Symbols
                                                 0xe000, 0xe00a, // Pomicons
                                                 0xe0a0, 0xe0a2, // Powerline
                                                 0xe0a3, 0xe0a3, // Powerline Extra
                                                 0xe0b0, 0xe0b3, // Powerline
                                                 0xe0b4, 0xe0c8, // Powerline Extra
                                                 0xe0ca, 0xe0ca, // Powerline Extra
                                                 0xe0cc, 0xe0d4, // Powerline Extra
                                                 0xe200, 0xe2a9, // Font Awesome Extension
                                                 0xe300, 0xe3e3, // Weather Icons
                                                 0xe5fa, 0xe6b1, // Seti-UI + Custom
                                                 0xe700, 0xe7c5, // Devicons
                                                 0xea60, 0xebeb, // Codicons
                                                 0xf000, 0xf2e0, // Font Awesome
                                                 0xf300, 0xf372, // Font Logos
                                                 0xf400, 0xf532, // Octicons
                                                 // 0xf0001, 0xf1af0, // Material Design
                                                 0};

static const std::map<std::string_view, ImGuiCol_> IMGUI_STYLE_KEYS = {
    {"border", ImGuiCol_Border},
    {"border-shadow", ImGuiCol_BorderShadow},
    {"button", ImGuiCol_Button},
    {"button-active", ImGuiCol_ButtonActive},
    {"button-hovered", ImGuiCol_ButtonHovered},
    {"check-mark", ImGuiCol_CheckMark},
    {"child-background", ImGuiCol_ChildBg},
    {"docking-empty-background", ImGuiCol_DockingEmptyBg},
    {"docking-preview", ImGuiCol_DockingPreview},
    {"drag-drop-target", ImGuiCol_DragDropTarget},
    {"frame-background", ImGuiCol_FrameBg},
    {"frame-background-active", ImGuiCol_FrameBgActive},
    {"frame-background-hovered", ImGuiCol_FrameBgHovered},
    {"header", ImGuiCol_Header},
    {"header-active", ImGuiCol_HeaderActive},
    {"header-hovered", ImGuiCol_HeaderHovered},
    {"menu-bar-background", ImGuiCol_MenuBarBg},
    {"modal-window-dim-background", ImGuiCol_ModalWindowDimBg},
    {"nav-highlight", ImGuiCol_NavHighlight},
    {"nav-windowing-background", ImGuiCol_NavWindowingHighlight},
    {"nav-windowing-highlight", ImGuiCol_NavWindowingHighlight},
    {"plot-histogram", ImGuiCol_PlotHistogram},
    {"plot-histogram-hovered", ImGuiCol_PlotHistogramHovered},
    {"plot-lines", ImGuiCol_PlotLines},
    {"plot-lines-hovered", ImGuiCol_PlotLinesHovered},
    {"popup-background", ImGuiCol_PopupBg},
    {"resize-grip", ImGuiCol_ResizeGrip},
    {"resize-grip-active", ImGuiCol_ResizeGripActive},
    {"resize-grip-hovered", ImGuiCol_ResizeGripHovered},
    {"scrollbar-background", ImGuiCol_ScrollbarBg},
    {"scrollbar-grab", ImGuiCol_ScrollbarGrab},
    {"scrollbar-grab-active", ImGuiCol_ScrollbarGrabActive},
    {"scrollbar-grab-hovered", ImGuiCol_ScrollbarGrabHovered},
    {"separator", ImGuiCol_Separator},
    {"separator-active", ImGuiCol_SeparatorActive},
    {"separator-hovered", ImGuiCol_SeparatorHovered},
    {"slider-grab", ImGuiCol_SliderGrab},
    {"slider-grab-active", ImGuiCol_SliderGrabActive},
    {"tab", ImGuiCol_Tab},
    {"tab-active", ImGuiCol_TabActive},
    {"tab-hovered", ImGuiCol_TabHovered},
    {"tab-unfocused", ImGuiCol_TabUnfocused},
    {"tab-unfocused-active", ImGuiCol_TabUnfocusedActive},
    {"table-border-light", ImGuiCol_TableBorderLight},
    {"table-border-strong", ImGuiCol_TableBorderStrong},
    {"table-header-background", ImGuiCol_TableHeaderBg},
    {"table-row-background", ImGuiCol_TableRowBg},
    {"table-row-background-alt", ImGuiCol_TableRowBgAlt},
    {"text", ImGuiCol_Text},
    {"text-disabled", ImGuiCol_TextDisabled},
    {"text-selected-background", ImGuiCol_TextSelectedBg},
    {"title-background", ImGuiCol_TitleBg},
    {"title-background-active", ImGuiCol_TitleBgActive},
    {"title-background-collapse", ImGuiCol_TitleBgCollapsed},
    {"window-background", ImGuiCol_WindowBg},
};

static const std::map<std::string_view, ImPlotCol_> IMPLOT_STYLE_KEYS = {
    {"axis-bg", ImPlotCol_AxisBg},
    {"axis-bg-active", ImPlotCol_AxisBgActive},
    {"axis-bg-hovered", ImPlotCol_AxisBgHovered},
    {"axis-grid", ImPlotCol_AxisGrid},
    {"axis-text", ImPlotCol_AxisText},
    {"axis-tick", ImPlotCol_AxisTick},
    {"crosshairs", ImPlotCol_Crosshairs},
    {"error-bar", ImPlotCol_ErrorBar},
    {"fill", ImPlotCol_Fill},
    {"frame-bg", ImPlotCol_FrameBg},
    {"inlay-text", ImPlotCol_InlayText},
    {"legend-bg", ImPlotCol_LegendBg},
    {"legend-border", ImPlotCol_LegendBorder},
    {"legend-text", ImPlotCol_LegendText},
    {"line", ImPlotCol_Line},
    {"marker-fill", ImPlotCol_MarkerFill},
    {"marker-outline", ImPlotCol_MarkerOutline},
    {"plot-bg", ImPlotCol_PlotBg},
    {"plot-border", ImPlotCol_PlotBorder},
    {"selection", ImPlotCol_Selection},
    {"title-text", ImPlotCol_TitleText},
};

ImFont *astri::ui::Style::add_font(std::string_view name, std::filesystem::path file, float size) {
  LOG_ZONE();

  LOG_DEBUG("Loading font{} ({:.0f}pt): {}", name, size, file);

  ImFontConfig config;
  fmt::format_to_n(config.Name, sizeof(config.Name) / sizeof(config.Name[0]), "{} {:.0f}pt", name,
                   size);

  ImFont *font = io->Fonts->AddFontFromFileTTF(file.c_str(), size, &config, NERD_FONT_GLYPH_RANGES);
  if (font == nullptr) {
    LOG_WARN("Failed to load font{} ({:.0f}pt): {}", name, size, file);
  }
  return font;
}

ImFont *astri::ui::Style::add_font(std::string_view name, const char *data, float size) {
  LOG_ZONE();

  LOG_DEBUG("Loading font{} ({:.0f}pt)", name, size);

  ImFontConfig config;
  fmt::format_to_n(config.Name, sizeof(config.Name) / sizeof(config.Name[0]), "{} {:.0f}pt", name,
                   size);

  ImFont *font =
      io->Fonts->AddFontFromMemoryCompressedBase85TTF(data, size, &config, NERD_FONT_GLYPH_RANGES);
  if (font == nullptr) {
    LOG_WARN("Failed to load font{} ({:.0f}pt)", name, size);
  }
  return font;
}

bool astri::ui::Style::load_fonts(float size) {
  LOG_ZONE();

#ifdef __EMSCRIPTEN__
  // clang-format off
  regular = add_font("Iosevka", std::filesystem::path("/assets/fonts/IosevkaNerdFont-Regular.ttf"), size);
  bold = add_font("Iosevka Bold", std::filesystem::path("/assets/fonts/IosevkaNerdFont-Bold.ttf"), size);
  italic = add_font("Iosevka Italic", std::filesystem::path("/assets/fonts/IosevkaNerdFont-Italic.ttf"), size);
  bold_italic = add_font("Iosevka Bold Italic", std::filesystem::path("/assets/fonts/IosevkaNerdFont-BoldItalic.ttf"), size);
  // clang-format on
#else
  regular = add_font("Iosevka", iosevkanerdfont_regular_compressed_data_base85, size);
  bold = add_font("Iosevka Bold", iosevkanerdfont_bold_compressed_data_base85, size);
  italic = add_font("Iosevka Italic", iosevkanerdfont_italic_compressed_data_base85, size);
  bold_italic =
      add_font("Iosevka Bold Italic", iosevkanerdfont_bolditalic_compressed_data_base85, size);
#endif

  return bold != nullptr && italic != nullptr && bold_italic != nullptr && regular != nullptr;
}

void astri::ui::Style::set_theme(std::filesystem::path file) {
  LOG_ZONE();

  if (std::filesystem::exists(file)) {
    std::ifstream stream(file);
    nlohmann::json config;
    stream >> config;
    set_style(config);
  } else {
    LOG_WARN("Theme file not found: {}", file);
  }
}

void astri::ui::Style::set_theme(const std::string &theme) {
  LOG_ZONE();
  if (theme == "catppuccin-frappe") {
    LOG_DEBUG("Loading theme: catppuccin-frappe");
    // set_theme(std::string(reinterpret_cast<const char *>(catppuccin_frappe_data),
    //                       catppuccin_frappe_size));
    return;
  }

  set_style(nlohmann::json::parse(theme));
}

bool astri::ui::Style::set_style(const nlohmann::json &config) {
  LOG_ZONE();

  const auto convert = [](const std::string &value) -> ImColor {
    return ImColor(std::stoul(value.substr(1, 2), nullptr, 16) / 255.0f,
                   std::stoul(value.substr(3, 2), nullptr, 16) / 255.0f,
                   std::stoul(value.substr(5, 2), nullptr, 16) / 255.0f,
                   value.size() == 9 ? std::stoul(value.substr(7, 2), nullptr, 16) / 255.0f : 1.0f);
  };

  if (config.contains("colors")) {
    const auto &colors = config["colors"];

    if (colors.contains("imgui")) {
      ImGuiStyle &style = ImGui::GetStyle();
      for (auto &[key, value] : colors["imgui"].items()) {
        if (value.is_string()) {
          const auto v = convert(value.get<std::string>());
          style.Colors[IMGUI_STYLE_KEYS.at(key)] = convert(value.get<std::string>());
        } else {
          LOG_WARN("Invalid color value for key: imgui.{}", key);
        }
      }
    }

    if (colors.contains("implot")) {
      ImPlotStyle &style = ImPlot::GetStyle();
      for (auto &[key, value] : colors["implot"].items()) {
        if (value.is_string()) {
          style.Colors[IMPLOT_STYLE_KEYS.at(key)] = convert(value.get<std::string>());
        } else {
          LOG_WARN("Invalid color value for key: implot.{}", key);
        }
      }
    }
  }

  return true;
}
