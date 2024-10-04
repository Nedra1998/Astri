#include "ui/style.hpp"

#include <imgui.h>

#include "log.hpp"

#ifndef __EMSCRIPTEN__
#  include "iosevka.h"
#  include "iosevka_bold.h"
#  include "iosevka_bold_italic.h"
#  include "iosevka_italic.h"
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
  regular = add_font("Iosevka", iosevka_compressed_data_base85, size);
  bold = add_font("Iosevka Bold", iosevka_bold_compressed_data_base85, size);
  italic = add_font("Iosevka Italic", iosevka_italic_compressed_data_base85, size);
  bold_italic = add_font("Iosevka Bold Italic", iosevka_bold_italic_compressed_data_base85, size);
#endif

  return bold != nullptr && italic != nullptr && bold_italic != nullptr && regular != nullptr;
}
