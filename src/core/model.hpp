#ifndef INCLUDE_CORE_MODEL_HPP_
#define INCLUDE_CORE_MODEL_HPP_

#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

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

  std::vector<GeneralPertubations> load(std::string_view group);
} // namespace astri::core

#endif // INCLUDE_CORE_MODEL_HPP_
