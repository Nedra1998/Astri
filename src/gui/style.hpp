#pragma once

#include <imgui.h>
#include <imgui_markdown.h>
#include <magic_enum.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

namespace astri::gui {
class Style {
public:
  Style(const Style &) = delete;
  void operator=(const Style &) = delete;

  static Style &get() {
    static Style instance;
    return instance;
  }

  enum FontStyle { REGULAR, BOLD, ITALIC, BOLD_ITALIC };

  void add_font(std::string_view key, const FontStyle &style,
                float font_size = 16.0f);
  inline void add_font(const FontStyle &style, float font_size = 16.0f) {
    add_font(magic_enum::enum_name(style), style, font_size);
  }

  inline ImFont *get_font(std::string_view key) const {
    if (std::unordered_map<std::string_view, ImFont *>::const_iterator it =
            fonts_.find(key);
        it != fonts_.end()) {
      return it->second;
    } else {
      return nullptr;
    }
  }

  inline ImFont *regular() const {
    if (std::unordered_map<std::string_view, ImFont *>::const_iterator it =
            fonts_.find(magic_enum::enum_name(FontStyle::REGULAR));
        it != fonts_.end()) {
      return it->second;
    } else {
      return nullptr;
    }
  }

  inline ImFont *bold() const {
    if (std::unordered_map<std::string_view, ImFont *>::const_iterator it =
            fonts_.find(magic_enum::enum_name(FontStyle::BOLD));
        it != fonts_.end()) {
      return it->second;
    } else {
      return nullptr;
    }
  }

  inline ImFont *italic() const {
    if (std::unordered_map<std::string_view, ImFont *>::const_iterator it =
            fonts_.find(magic_enum::enum_name(FontStyle::ITALIC));
        it != fonts_.end()) {
      return it->second;
    } else {
      return nullptr;
    }
  }

  inline ImFont *bold_italic() const {
    if (std::unordered_map<std::string_view, ImFont *>::const_iterator it =
            fonts_.find(magic_enum::enum_name(FontStyle::BOLD_ITALIC));
        it != fonts_.end()) {
      return it->second;
    } else {
      return nullptr;
    }
  }

  inline const ImGui::MarkdownConfig &md_config() const { return md_config_; }
  inline void markdown(const std::string_view text) {
    ImGui::Markdown(text.data(), text.length(), md_config_);
  }

protected:
  static void md_link_callback(ImGui::MarkdownLinkCallbackData data);
  static void md_format_callback(const ImGui::MarkdownFormatInfo &info,
                                 bool start);

private:
  Style() {
    io_                        = &ImGui::GetIO();
    md_config_.tooltipCallback = ImGui::defaultMarkdownTooltipCallback;
    md_config_.linkCallback    = md_link_callback;
    md_config_.formatCallback  = md_format_callback;
  }

  ImGuiIO *io_ = nullptr;
  ImGui::MarkdownConfig md_config_;

  std::unordered_map<std::string_view, ImFont *> fonts_;
};
} // namespace astri::gui
