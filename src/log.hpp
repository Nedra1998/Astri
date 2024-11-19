#ifndef INCLUDE_SRC_LOG_HPP_
#define INCLUDE_SRC_LOG_HPP_

#include <fmt/ostream.h>
#include <fmt/std.h>
#include <glad/gl.h>
#include <spdlog/spdlog.h>
#include <tracy/Tracy.hpp>
#include <tracy/TracyOpenGL.hpp>

#define LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define LOG_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

#define LOGGER_TRACE(...) SPDLOG_LOGGER_TRACE(__VA_ARGS__)
#define LOGGER_DEBUG(...) SPDLOG_LOGGER_DEBUG(__VA_ARGS__)
#define LOGGER_INFO(...) SPDLOG_LOGGER_INFO(__VA_ARGS__)
#define LOGGER_WARN(...) SPDLOG_LOGGER_WARN(__VA_ARGS__)
#define LOGGER_ERROR(...) SPDLOG_LOGGER_ERROR(__VA_ARGS__)
#define LOGGER_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(__VA_ARGS__)

#define MARK_FRAME() FrameMark
#define CO_ZONE()                                                                                  \
  TracyFiberEnter(__PRETTY_FUNCTION__);                                                            \
  ZoneScopedN(__PRETTY_FUNCTION__)
#define CO_AWAIT(x)                                                                                \
  TracyFiberLeave;                                                                                 \
  co_await x;                                                                                      \
  TracyFiberEnter(__PRETTY_FUNCTION__)
#define CO_RETURN(x)                                                                               \
  TracyFiberLeave;                                                                                 \
  co_return x
#define LOG_ZONE() ZoneScopedN(__PRETTY_FUNCTION__)
#define LOG_ZONEN(name) ZoneScopedN(name)
#define GPU_ZONE(name) TracyGpuZone(name)
#define APP_INFO(...)                                                                              \
  do {                                                                                             \
    const std::string _str = fmt::format(__VA_ARGS__);                                             \
    TracyAppInfo(_str.c_str(), _str.size());                                                       \
  } while (0)

#define ASSERT(x) (!(!(x) && (LOG_CRITICAL("Assertion failed: {}", #x), true)))

namespace astri::log {
  bool initialize();
  std::shared_ptr<spdlog::logger> get(std::string_view name);
} // namespace astri::log

#endif // INCLUDE_SRC_LOG_HPP_
