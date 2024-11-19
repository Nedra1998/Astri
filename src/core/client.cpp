#include "core/client.hpp"

#include "ecs.hpp"
#include "log.hpp"

bool astri::core::Client::load(std::vector<std::string_view> groups) {
  LOG_ZONE();

  std::unordered_map<std::string_view, GeneralPertubations> elements;

  bool success = true;
  for (const auto &group : groups) {
    if (auto res = get(group)) {
      LOG_DEBUG("Loaded {} elements in {}", res->size(), group);
      for (const auto &element : *res) {
        elements[element.object_id] = element;
      }
    } else {
      LOG_ERROR("Failed to load elements for group: {}", group);
      success = false;
    }
  }

  LOG_DEBUG("Loaded {} unique elements", elements.size());

  for (const auto &[object_id, element] : elements) {
    ecs::WORLD.entity().set(element);
  }
  return success;
}

std::optional<std::vector<astri::GeneralPertubations>>
astri::core::Client::get(std::string_view group) {
  LOG_ZONE();

  const std::string path = fmt::format("/NORAD/elements/gp.php?GROUP={}&FORMAT=JSON", group);

  LOG_INFO("GET {}", path);

  return cache_(
      path,
      [&](const std::string_view &key) -> std::optional<std::vector<GeneralPertubations>> {
        LOG_ZONE();
        LOG_TRACE("Cache Miss: {}", key);
        if (auto res = client_.Get(key.data())) {
          try {
            nlohmann::json j = nlohmann::json::parse(res->body);
            std::vector<GeneralPertubations> elements;
            elements.reserve(j.size());
            for (const auto &element : j) {
              elements.push_back(element);
            }
            return elements;
            // return j.get<std::vector<GeneralPertubations>>();
          } catch (nlohmann::json::parse_error &err) {
            LOG_ERROR("JSON Parse Error: {}", err.what());
          }
        } else {
          LOG_ERROR("HTTP Error: {}", httplib::to_string(res.error()));
        }
        return std::nullopt;
      },
      std::chrono::hours(1));
}
