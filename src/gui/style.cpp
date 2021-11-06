#include "gui/style.hpp"

#include <imgui.h>
#include <magic_enum.hpp>
#include <string>
#include <string_view>

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include "Shellapi.h"
#  include <Windows.h>
#endif

#include "iosevka-bold.hpp"
#include "iosevka-bolditalic.hpp"
#include "iosevka-italic.hpp"
#include "iosevka-regular.hpp"
#include "logging.hpp"

void astri::gui::Style::add_font(std::string_view key, const FontStyle &style,
                                 float font_size) {
  if (io_ == nullptr) {
    LWARN("imgui", "Failed to load font ImGui IO context has not been created");
    return;
  }

  std::string style_name = std::string(magic_enum::enum_name(style));
  bool new_word          = true;
  std::transform(style_name.begin(), style_name.end(), style_name.begin(),
                 [new_word](char ch) mutable -> char {
                   if (new_word) {
                     new_word = false;
                     return ch;
                   } else if (ch == '_') {
                     new_word = true;
                     return ' ';
                   } else {
                     return static_cast<char>(std::tolower(ch));
                   }
                 });

  ImFontConfig config;
  fmt::format_to_n(config.Name, sizeof(config.Name) / sizeof(config.Name[0]),
                   "Iosevka {} {:.0f}px", style_name, font_size);
  LDEBUG("imgui", "Loading embeded font Iosevka {} ({:.0f}px)", style_name,
         font_size);

  ImFont *font = nullptr;

  switch (style) {
  case FontStyle::REGULAR:
    font = io_->Fonts->AddFontFromMemoryCompressedTTF(
        iosevka_regular_compressed_data, iosevka_regular_compressed_size,
        font_size, &config);
    break;
  case FontStyle::BOLD:
    font = io_->Fonts->AddFontFromMemoryCompressedTTF(
        iosevka_bold_compressed_data, iosevka_bold_compressed_size, font_size,
        &config);
    break;
  case FontStyle::ITALIC:
    font = io_->Fonts->AddFontFromMemoryCompressedTTF(
        iosevka_italic_compressed_data, iosevka_italic_compressed_size,
        font_size, &config);
    break;
  case FontStyle::BOLD_ITALIC:
    font = io_->Fonts->AddFontFromMemoryCompressedTTF(
        iosevka_bolditalic_compressed_data, iosevka_bolditalic_compressed_size,
        font_size, &config);
    break;
  }

  if (font != nullptr) {
    fonts_.insert({key, font});
  } else {
    LWARN("imgui", "Failed to load embeded font Iosevka {}", style_name);
  }
}

// void astri::gui::Style::md_link_callback(ImGui::MarkdownLinkCallbackData
// data) {
//   std::string url(data.link, data.linkLength);
//   if (!data.isImage) {
//     LDEBUG("imgui", "Opening url \"{}\" in default browser", url);
// #if defined(_WIN32)
//     ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
// #else
//     system(("xdg-open " + std::string(url) + "&").c_str());
// #endif
//   }
// }
// void astri::gui::Style::md_format_callback(
//     const ImGui::MarkdownFormatInfo &info, bool start) {
//   switch (info.type) {
//   case ImGui::MarkdownFormatType::NORMAL_TEXT: {
//     break;
//   }
//   case ImGui::MarkdownFormatType::EMPHASIS: {
//     if (info.level == 1) {
//       ImFont *font = Style::get().italic();
//       if (start && font != nullptr)
//         ImGui::PushFont(font);
//       else if (font != nullptr)
//         ImGui::PopFont();
//     } else {
//       ImFont *font = Style::get().bold();
//       if (start && font != nullptr)
//         ImGui::PushFont(font);
//       else if (font != nullptr)
//         ImGui::PopFont();
//     }
//     break;
//   }
//   case ImGui::MarkdownFormatType::HEADING: {
//     ImFont *font = nullptr;
//     if (info.level > ImGui::MarkdownConfig::NUMHEADINGS) {
//       font = Style::get().get_font("H1");
//     } else {
//       font = Style::get().get_font(fmt::format("H{}", info.level));
//     }

//     if (start) {
//       if (font != nullptr)
//         ImGui::PushFont(font);
//     } else {
//       if (info.level <= 2)
//         ImGui::Separator();
//       if (font != nullptr)
//         ImGui::PopFont();
//     }
//     break;
//   }
//   case ImGui::MarkdownFormatType::UNORDERED_LIST: {
//     break;
//   }
//   case ImGui::MarkdownFormatType::LINK: {
//     if (start) {
//       ImGui::PushStyleColor(ImGuiCol_Text,
//                             ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
//     } else {
//       ImGui::PopStyleColor();
//       if (info.itemHovered) {
//         ImGui::UnderLine(ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
//       } else {
//         ImGui::UnderLine(ImGui::GetStyle().Colors[ImGuiCol_Button]);
//       }
//     }
//     break;
//   }
//   }
// }
