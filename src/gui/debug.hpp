#pragma once

#include <chrono>
#include <memory>
#include <spdlog/sinks/ringbuffer_sink.h>
#include <spdlog/spdlog.h>

#include "gui/window.hpp"
#include "logging.hpp"
#include "spdlog/details/log_msg_buffer.h"
#include "spdlog/sinks/base_sink.h"

namespace astri::debug {
class FramerateWindow : public gui::Window {
public:
  FramerateWindow(std::size_t buffer = 1200) : Window("Framerate") {
    data.reserve(buffer);
  }

  void render() override;
  void update() override;

private:
  std::chrono::high_resolution_clock::time_point last_update;
  std::vector<std::array<double, 5>> data;
  std::array<double, 3> data_next;
  std::size_t data_offset      = 0;
  std::size_t data_samplecount = 0;
  float lookback               = 60.0f;
};

template <typename Mutex>
class forward_sink final : public spdlog::sinks::base_sink<Mutex> {
public:
  explicit forward_sink(std::vector<spdlog::details::log_msg_buffer> *buffer,
                        std::size_t *offset)
      : buffer_{buffer}, offset_{offset} {
    buffer_->clear();
    *offset_ = 0;
  }

protected:
  void sink_it_(const spdlog::details::log_msg &msg) override {
    if (buffer_->size() < buffer_->capacity()) {
      buffer_->push_back(spdlog::details::log_msg_buffer{msg});
    } else {
      buffer_->at(*offset_) = spdlog::details::log_msg_buffer{msg};
      *offset_              = (*offset_ + 1) % buffer_->capacity();
    }
  }

  void flush_() override {}

private:
  std::vector<spdlog::details::log_msg_buffer> *buffer_;
  std::size_t *offset_;
};

using forward_sink_mt = forward_sink<std::mutex>;
using forward_sink_st = forward_sink<spdlog::details::null_mutex>;

class LoggingWindow : public gui::Window {
public:
  LoggingWindow(std::size_t size = 512)
      : Window("Logging"), buffer(size),
        sink(std::make_shared<forward_sink_mt>(&buffer, &offset)) {
    if (astri::logging::sink != nullptr)
      astri::logging::sink->add_sink(sink);
  }

  void render() override;

private:
  std::chrono::high_resolution_clock::time_point last_update;
  std::vector<spdlog::details::log_msg_buffer> buffer;
  std::size_t offset;
  std::shared_ptr<forward_sink_mt> sink;
};
} // namespace astri::debug
