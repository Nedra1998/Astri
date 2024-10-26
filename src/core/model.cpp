#include "core/model.hpp"

#include <chrono>

#include <fmt/format.h>
#include <httplib.h>

#include "log.hpp"
#include "util/cache.hpp"

using namespace std::chrono_literals;

std::vector<std::string> split(std::string s, const std::string &delimiter) {
  std::vector<std::string> tokens;
  size_t pos = 0;
  std::string token;
  while ((pos = s.find(delimiter)) != std::string::npos) {
    token = s.substr(0, pos);
    tokens.push_back(token);
    s.erase(0, pos + delimiter.length());
  }
  tokens.push_back(s);

  return tokens;
}

std::vector<astri::core::GeneralPertubations> astri::core::load(std::string_view group) {

  util::Cache &cache = util::Cache::get();

  std::function<std::string()> loader = [&group]() -> std::string {
    httplib::Client client("https://celestrak.org");
    if (auto res = client.Get(fmt::format("/NORAD/elements/gp.php?GROUP={}&FORMAT=CSV", group))) {
      return res->body;
    } else {
      auto err = res.error();
      LOG_ERROR("HTTP Error: {}", httplib::to_string(err));
      return "";
    }
  };

  std::string body = cache(group, loader, 300s);
  if (body.empty())
    return {};

  std::vector<std::string> lines = split(body, "\n");
  lines.erase(lines.begin());

  std::vector<GeneralPertubations> sattilites = {};
  for (const auto &line : lines) {
    std::vector<std::string> segments = split(line, ",");

    std::chrono::system_clock::time_point tp;
    std::istringstream ss(segments[3]);
    ss >> std::chrono::parse("%FT%T", tp);

    sattilites.push_back(GeneralPertubations{
        .object_name = segments[0],
        .object_id = segments[1],
        .epoch = tp,
        .mean_motion = std::stod(segments[4]),
        .eccentricity = std::stod(segments[5]),
        .inclination = std::stod(segments[6]),
        .ra_of_asc_node = std::stod(segments[7]),
        .arg_of_pericenter = std::stod(segments[8]),
        .mean_anomaly = std::stod(segments[9]),
        .classification_type = static_cast<Classification>(segments[10][0]),
        .norad_cat_id = static_cast<std::uint32_t>(std::stoul(segments[11])),
        .element_set_no = static_cast<std::uint32_t>(std::stoul(segments[12])),
        .rev_at_epoch = std::stoull(segments[13]),
        .bstar = std::stod(segments[14]),
        .mean_motion_dot = std::stod(segments[15]),
        .mean_motion_ddot = std::stod(segments[16]),
    });
  }

  return sattilites;
}
