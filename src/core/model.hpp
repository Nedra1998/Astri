#ifndef INCLUDE_CORE_MODEL_HPP_
#define INCLUDE_CORE_MODEL_HPP_

#include <chrono>
#include <cstdint>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "log.hpp"

namespace astri::core {
  enum class Classification : std::uint8_t { UNCLASSIFIED = 'U', CLASSIFIED = 'C', SECRET = 'S' };

  struct GeneralPertubations {
    std::string object_name;
    std::string object_id;
    std::chrono::system_clock::time_point epoch;
    double mean_motion;
    double eccentricity;
    double inclination;
    double ra_of_asc_node;
    double arg_of_pericenter;
    double mean_anomaly;
    Classification classification_type;
    std::uint32_t norad_cat_id;
    std::uint32_t element_set_no;
    std::uint64_t rev_at_epoch;
    double bstar;
    double mean_motion_dot;
    double mean_motion_ddot;
  };

  struct Group {
    std::string name;
    std::vector<std::size_t> members;
  };

  class Datastore {
  public:
    static Datastore &get() {
      static Datastore instance;
      return instance;
    }

    inline void insert(std::string_view group,
                       const std::vector<GeneralPertubations> &group_satellites) {
      std::vector<std::size_t> members;

      for (auto &satellite : group_satellites) {
        auto it = std::find_if(satellites.begin(), satellites.end(),
                               [&satellite](const GeneralPertubations &s) {
                                 return s.object_id == satellite.object_id;
                               });
        if (it == satellites.end()) {
          members.push_back(satellites.size());
          satellites.push_back(satellite);
        } else {
          members.push_back(std::distance(satellites.begin(), it));
        }
      }

      if (std::find_if(groups.begin(), groups.end(),
                       [group](const Group &g) { return g.name == group; }) == groups.end()) {
        groups.push_back({std::string(group), members});
      }
    }

    std::vector<GeneralPertubations> satellites;
    std::vector<Group> groups;
  };

  inline std::chrono::system_clock::time_point parse_epoch(const std::string &epoch) {
    std::istringstream ss(epoch);
    std::chrono::system_clock::time_point timepoint;
    ss >> std::chrono::parse("%Y-%m-%dT%H:%M:%S", timepoint);

    if (ss.peek() == '.') {
      ss.ignore();
      uint microseconds;
      ss >> microseconds;
      timepoint += std::chrono::microseconds(microseconds);
    }
    return timepoint;
  }

  inline void to_json(nlohmann::json &j, const GeneralPertubations &p) {
    std::string epoch = fmt::format("{:%Y-%m-%dT%H:%M:%S}", p.epoch);

    j = nlohmann::json{
        {"OBJECT_NAME", p.object_name},
        {"OBJECT_ID", p.object_id},
        {"EPOCH", epoch},
        {"MEAN_MOTION", p.mean_motion},
        {"ECCENTRICITY", p.eccentricity},
        {"INCLINATION", p.inclination},
        {"RA_OF_ASC_NODE", p.ra_of_asc_node},
        {"ARG_OF_PERICENTER", p.arg_of_pericenter},
        {"MEAN_ANOMALY", p.mean_anomaly},
        {"CLASSIFICATION_TYPE", std::string(static_cast<char>(p.classification_type), 1)},
        {"NORAD_CAT_ID", p.norad_cat_id},
        {"ELEMENT_SET_NO", p.element_set_no},
        {"REV_AT_EPOCH", p.rev_at_epoch},
        {"BSTAR", p.bstar},
        {"MEAN_MOTION_DOT", p.mean_motion_dot},
        {"MEAN_MOTION_DDOT", p.mean_motion_ddot}};
  }

  inline void from_json(const nlohmann::json &j, GeneralPertubations &p) {
    std::string epoch;
    std::string classification_type;
    j.at("OBJECT_NAME").get_to(p.object_name);
    j.at("OBJECT_ID").get_to(p.object_id);
    j.at("EPOCH").get_to(epoch);
    j.at("MEAN_MOTION").get_to(p.mean_motion);
    j.at("ECCENTRICITY").get_to(p.eccentricity);
    j.at("INCLINATION").get_to(p.inclination);
    j.at("RA_OF_ASC_NODE").get_to(p.ra_of_asc_node);
    j.at("ARG_OF_PERICENTER").get_to(p.arg_of_pericenter);
    j.at("MEAN_ANOMALY").get_to(p.mean_anomaly);
    j.at("CLASSIFICATION_TYPE").get_to(classification_type);
    j.at("NORAD_CAT_ID").get_to(p.norad_cat_id);
    j.at("ELEMENT_SET_NO").get_to(p.element_set_no);
    j.at("REV_AT_EPOCH").get_to(p.rev_at_epoch);
    j.at("BSTAR").get_to(p.bstar);
    j.at("MEAN_MOTION_DOT").get_to(p.mean_motion_dot);
    j.at("MEAN_MOTION_DDOT").get_to(p.mean_motion_ddot);

    p.epoch = parse_epoch(epoch);
    p.classification_type = static_cast<Classification>(classification_type[0]);
  }
} // namespace astri::core

#endif // INCLUDE_CORE_MODEL_HPP_
