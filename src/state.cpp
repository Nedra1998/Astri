#include "state.hpp"

#include <random>

#include <fmt/chrono.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <sgp4/sgp4.hpp>

#include "ecs.hpp"
#include "gl/object.hpp"
#include "ui/colors.hpp"

std::string_view identify_group(std::string_view name) {
  // Get the first word of the name (up to the first space or '-')
  const std::size_t space_pos = name.find_first_of(" -");
  if (space_pos == std::string_view::npos) {
    return "";
  }

  // Check that the prefix is only letters
  const std::string_view prefix = name.substr(0, space_pos);
  if (!std::all_of(prefix.begin(), prefix.end(), ::isalpha)) {
    return "";
  }

  return prefix;
}

void astri::SimulationState::setup(gl::Object &object) {
  // Initialize the now time to the current time
  now = std::chrono::high_resolution_clock::now();

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> angle_dist(0.0f, glm::two_pi<float>());

  std::vector<SatelliteState> satellite_states;

  std::size_t idx = 0;
  astri::ecs::WORLD.query<sgp4::ElementSet>().each([&](flecs::entity e, sgp4::ElementSet &gp) {
    // Initialize the SGP4 element set and perform the first SGP4 calculation
    // to populate the initial position and velocity
    double tsince = std::chrono::duration<double>(now - gp.epoch).count();
    double r[3], v[3];
    sgp4::init(sgp4::GravityModel::WGS72, gp);
    sgp4::sgp4(gp, tsince / 60.0, r, v);

    // Identify the grouping for the satellite based on its object name
    const std::string_view group = identify_group(gp.object_name);
    const glm::vec4 color = !group.empty() ? ui::colors::group(group) : ui::colors::random();

    // Push the satellite state to the vector for later use in updates
    satellite_states.push_back({idx, e.id(), true, std::string(group), color});

    // Define the new instance for the satellite
    object.instance(
        idx++,
        glm::scale(glm::translate(glm::mat4(1.0), glm::vec3(r[0], r[1], r[2])), glm::vec3(scale)),
        color);
  });

  for (const auto &ss : satellite_states) {
    ecs::WORLD.entity(ss.entity_id).set(ss);
  }

  LOG_INFO("Initialized {} satellite instances", idx);
}

void astri::SimulationState::update(float delta, gl::Object &object) {
  if (!paused) {
    now += std::chrono::milliseconds(speed_multiplier * static_cast<long>(delta * 1000.0f));
  }

  std::size_t idx = 0;
  astri::ecs::WORLD.query<sgp4::ElementSet, SatelliteState>().each(
      [&](sgp4::ElementSet &gp, SatelliteState &ss) {
        // If not visible, scale down to 0 to hide the satellite and skip the SGP4 calculation
        if (!ss.visible) {
          object.instance(ss.idx, glm::mat4(0.0f));
        }

        // Calculate the new position of the satellite based on the current time
        // and the SGP4 element set
        double tsince = std::chrono::duration<double>(now - gp.epoch).count();
        double r[3], v[3];
        sgp4::sgp4(gp, tsince / 60.0, r, v);

        // Update the transformation matrix for the satellite instance.
        object.instance(idx++,
                        glm::scale(glm::translate(glm::mat4(1.0), glm::vec3(r[0], r[1], r[2])),
                                   glm::vec3(scale)),
                        ss.color);
      });
}
