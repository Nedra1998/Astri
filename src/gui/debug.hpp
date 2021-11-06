#pragma once

#include "gui/window.hpp"
#include <chrono>

namespace astri::debug {
class FramerateWindow : public gui::Window {
public:
  FramerateWindow(std::size_t buffer = 1200)
      : Window("Framerate") {
      data.reserve(buffer);
  }

  void render() override;
  void update() override;

private:
  std::chrono::high_resolution_clock::time_point last_update;
  std::vector<std::array<double, 5>> data;
  std::array<double, 3> data_next;
  std::size_t data_offset = 0;
  std::size_t data_samplecount = 0;
  float lookback = 60.0f;
};

class LoggingWindow : public gui::Window {
public:
  LoggingWindow() : Window("Logging") {}

  void render() override;
};
} // namespace astri::debug
