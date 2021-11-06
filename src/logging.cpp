#include "logging.hpp"

#include <memory>
#include <spdlog/sinks/dist_sink.h>

namespace astri::logging {
std::shared_ptr<spdlog::sinks::dist_sink_mt> sink = nullptr;
}

void astri::logging::callback(int error, const char *text) {
#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_ERROR
  get("glfw")->log(spdlog::level::err, "({}): {}", error, text);
#endif
}
