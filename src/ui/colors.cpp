#include "ui/colors.hpp"

#include <random>
#include <unordered_map>

inline glm::vec4 hex_to_rgba(const std::string &hex) {
  unsigned int r, g, b;
  if (hex[0] == '#') {
    std::sscanf(hex.c_str(), "#%02x%02x%02x", &r, &g, &b);
    return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
  }
  return glm::vec4(1.0f); // Default to white if invalid format
}

glm::vec4 astri::ui::colors::random() {
  static std::mt19937 rng(std::random_device{}());
  static std::uniform_int_distribution<std::size_t> dist(0, COLORS.size() - 1);
  return hex_to_rgba(COLORS[dist(rng)]);
}

glm::vec4 astri::ui::colors::group(std::string_view group) {
  static std::unordered_map<std::string_view, glm::vec4> cache;
  if (auto it = cache.find(group); it != cache.end()) {
    return it->second;
  }

  const glm::vec4 color = random();
  cache[group] = color;
  return color;
}
