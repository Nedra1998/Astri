#ifndef INCLUDE_SRC_ECS_HPP_
#define INCLUDE_SRC_ECS_HPP_

#define FLECS_CPP
#define FLECS_META
#define FLECS_MODULE
#define FLECS_PIPELINE
#define FLECS_SYSTEM
#define FLECS_TIMER
#define FLECS_UNITS

#define FLECS_NO_APP
#define FLECS_NO_DOC
#define FLECS_NO_HTTP
#define FLECS_NO_JOURNAL
#define FLECS_NO_JSON
#define FLECS_NO_LOG
#define FLECS_NO_OS_API_IMPL
#define FLECS_NO_REST
#define FLECS_NO_SCRIPT
#define FLECS_NO_STATS

#include <flecs.h>

namespace astri::ecs {
  extern flecs::world WORLD;

  bool initialize();

  struct stats {
    stats(flecs::world &world);
  };
} // namespace astri::ecs

#endif // INCLUDE_SRC_ECS_HPP_
