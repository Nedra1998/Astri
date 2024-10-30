#include "ecs.hpp"

#include "gl/gl.hpp"
#include "log.hpp"

namespace astri::ecs {
  flecs::world WORLD;
}

bool astri::ecs::initialize() {
  LOG_ZONE();

  LOG_INFO("Flecs v{}.{}.{}", FLECS_VERSION_MAJOR, FLECS_VERSION_MINOR, FLECS_VERSION_PATCH);

  WORLD = flecs::world();

  WORLD.import <astri::ecs::stats>();

  return true;
}

astri::ecs::stats::stats(flecs::world &world) {
  world.module<stats>();

  static double last_time = 0.0;
  static std::size_t frame_count = 0;

  world.system("StatRecorder").kind(flecs::PreStore).run([&](flecs::iter &it) { frame_count++; });

  world.system("ReportStats").interval(1.0).kind(flecs::OnUpdate).run([&](flecs::iter &it) {
    double current_time = glfwGetTime();
    double delta_time = current_time - last_time;

    LOG_INFO("FPS: {:.0f} ({:.1f}ms)", frame_count / delta_time,
             (delta_time * 1000.0) / frame_count);

    last_time = current_time;
    frame_count = 0;
  });
}
