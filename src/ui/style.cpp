#include "ui/style.hpp"

#include "iosevkanerdfont_bold.h"
#include "iosevkanerdfont_bolditalic.h"
#include "iosevkanerdfont_italic.h"
#include "iosevkanerdfont_regular.h"
#include "log.hpp"

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

bool astri::ui::Style::load_fonts(float size) {
  LOG_ZONE();

  // clang-format off
  regular = load_font("Iosevka",                 iosevkanerdfont_regular_compressed_data_base85,    size);
  bold = load_font("Iosevka Bold",               iosevkanerdfont_bold_compressed_data_base85,       size);
  italic = load_font("Iosevka Italic",           iosevkanerdfont_italic_compressed_data_base85,     size);
  bold_italic = load_font("Iosevka Bold Italic", iosevkanerdfont_bolditalic_compressed_data_base85, size);
  // clang-format on

  return bold && italic && bold_italic && regular;
}

ImFont *astri::ui::Style::load_font(std::string_view name, const char *data, float size) {
  LOG_ZONE();

  LOG_DEBUG("Loading font {} ({:.0f}pt)", name, size);

  ImFontConfig config;
  fmt::format_to_n(config.Name, sizeof(config.Name) / sizeof(config.Name[0]), "{} {:.0f}pt", name,
                   size);

  ImFont *font =
      io->Fonts->AddFontFromMemoryCompressedBase85TTF(data, size, &config, NERD_FONT_GLYPH_RANGES);

  if (font == nullptr) {
    LOG_ERROR("Failed to load font {} ({:.0f}pt)", name, size);
  }

  return font;
}
