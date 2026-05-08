#ifndef INCLUDE_SRC_STATE_HPP_
#define INCLUDE_SRC_STATE_HPP_

#include <chrono>

#include "gl/object.hpp"
namespace astri {
  struct SatelliteState {
    std::size_t idx = 0;
    std::size_t entity_id = 0;

    bool visible = true;

    std::string group;
    glm::vec4 color = glm::vec4(1.0f);
  };

  struct SimulationState {
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();

    bool paused = false;
    int speed_multiplier = 1;
    float scale = 25.0f;

    static inline SimulationState &instance() {
      static SimulationState state;
      return state;
    }

    void setup(gl::Object &object);
    void update(float delta, gl::Object &object);
  };

} // namespace astri

#endif // INCLUDE_SRC_STATE_HPP_
