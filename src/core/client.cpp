#include "core/client.hpp"

#include <fstream>
#include <optional>

#include "log.hpp"

std::optional<std::vector<astri::core::GeneralPertubations>>
astri::core::Client::group(std::string_view group) {
  if (auto satellites = get(group)) {
    return satellites;
  }
  if (auto satellites = request(group)) {
    put(group, *satellites);
    return satellites;
  }
  return std::nullopt;
}

std::optional<std::vector<astri::core::GeneralPertubations>>
astri::core::Client::request(std::string_view group) {
  LOG_TRACE("GET /NORAD/elements/gp.php?GROUP={}&FORMAT=JSON", group);
  if (auto res = client_.Get(fmt::format("/NORAD/elements/gp.php?GROUP={}&FORMAT=JSON", group))) {

    try {
      std::vector<GeneralPertubations> satellites;
      nlohmann::json json = nlohmann::json::parse(res->body);
      for (auto &j : json) {
        satellites.push_back(j.get<GeneralPertubations>());
      }
      return satellites;
    } catch (nlohmann::json::parse_error &e) {
      LOG_ERROR("JSON Parse Error: {}", e.what());
      return std::nullopt;
    }
  } else {
    auto err = res.error();
    LOG_ERROR("HTTP Error: {}", httplib::to_string(err));
    return std::nullopt;
  }
}

std::optional<std::vector<astri::core::GeneralPertubations>>
astri::core::Client::get(std::string_view group) const {
  std::filesystem::path cache = cache_ / fmt::format("{}.cbor", group);
  if (!std::filesystem::exists(cache)) {
    return std::nullopt;
  } else if (std::chrono::clock_cast<std::chrono::system_clock>(
                 std::filesystem::last_write_time(cache)) +
                 ttl_ <=
             std::chrono::system_clock::now()) {
    std::filesystem::remove(cache);
    return std::nullopt;
  }

  try {
    std::vector<std::uint8_t> bytes;
    std::ifstream file(cache, std::ios_base::binary);
    if (!file.is_open()) {
      LOG_ERROR("Failed to open cache file: {}", cache.string());
      return std::nullopt;
    }

    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    bytes.resize(size);
    file.read(reinterpret_cast<char *>(bytes.data()), size);

    std::vector<GeneralPertubations> satellites;
    nlohmann::json json = nlohmann::json::from_cbor(bytes);
    for (auto &j : json) {
      satellites.push_back(j.get<GeneralPertubations>());
    }
    return satellites;
  } catch (nlohmann::json::parse_error &e) {
    LOG_ERROR("JSON Parse Error: {}", e.what());
    return std::nullopt;
  }
}

void astri::core::Client::put(std::string_view group,
                              std::vector<GeneralPertubations> &satellites) const {
  if (!std::filesystem::exists(cache_)) {
    std::filesystem::create_directories(cache_);
  }
  std::filesystem::path cache = cache_ / fmt::format("{}.cbor", group);
  try {
    nlohmann::json json;
    for (auto &satellite : satellites) {
      json.push_back(satellite);
    }
    std::vector<std::uint8_t> bytes = nlohmann::json::to_cbor(json);

    std::ofstream file(cache, std::ios_base::binary);
    if (!file.is_open()) {
      LOG_ERROR("Failed to open cache file: {}", cache.string());
      return;
    }
    file.write(reinterpret_cast<char *>(bytes.data()), bytes.size());
  } catch (nlohmann::json::parse_error &e) {
    LOG_ERROR("JSON Parse Error: {}", e.what());
  }
}
