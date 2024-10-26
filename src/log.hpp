#ifndef INCLUDE_SRC_LOG_HPP_
#define INCLUDE_SRC_LOG_HPP_

#include <fmt/ostream.h>
#include <fmt/std.h>
#include <spdlog/spdlog.h>

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

#define LOG_ZONE() SPDLOG_TRACE("{}", __PRETTY_FUNCTION__)

#define ASSERT(x) (!(!(x) && (LOG_CRITICAL("Assertion failed: {}", #x), true)))

#endif // INCLUDE_SRC_LOG_HPP_
