#include "gui/debug.hpp"
#include <chrono>
#include <imgui.h>
#include <implot.h>
#include <ratio>
#include <utility>

#include "logging.hpp"

void astri::debug::FramerateWindow::render() {
  ImGui::SliderFloat("Lookback", &lookback, 1.0f, 300.0f, "%.1f s");

  if (ImPlot::BeginPlot("##FrameratePlot", ImVec2(-1, -1))) {
    ImPlot::SetupAxis(ImAxis_X1, NULL, ImPlotAxisFlags_Time);
    ImPlot::SetupAxis(ImAxis_Y1, NULL, ImPlotAxisFlags_None);
    ImPlot::SetupAxis(ImAxis_Y2, NULL, ImPlotAxisFlags_None);

    ImPlot::SetupAxisLimits(ImAxis_Y1, 10, 20);
    ImPlot::SetupAxisLimits(ImAxis_Y2, 0, 120);

    if (data_offset == 0)
      ImPlot::SetupAxisLimits(ImAxis_X1, data[data.size() - 1][0] - lookback,
                              data[data.size() - 1][0], ImGuiCond_Always);
    else
      ImPlot::SetupAxisLimits(ImAxis_X1, data[data_offset - 1][0] - lookback,
                              data[data_offset - 1][0], ImGuiCond_Always);

    ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
    ImPlot::PlotShaded("FrameTimes (ms)", &data[0][0], &data[0][1], &data[0][3],
                       data.size(), data_offset, 5 * sizeof(double));
    ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
    ImPlot::PlotLine("FrameTimes (ms)", &data[0][0], &data[0][2], data.size(),
                     data_offset, 5 * sizeof(double));

    ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
    ImPlot::PlotLine("FPS", &data[0][0], &data[0][4], data.size(), data_offset,
                     5 * sizeof(double));
    ImPlot::EndPlot();
  }
}
void astri::debug::FramerateWindow::update() {
  const auto now = std::chrono::high_resolution_clock::now();
  const float t  = ImGui::GetIO().DeltaTime;

  if ((now - last_update) > std::chrono::milliseconds(250)) {
    const auto timestamp =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch());
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - last_update);

    if (data_samplecount != 0) {
      const std::array<double, 5> val = {
          static_cast<double>(timestamp.count()) / 1000.0,
          data_next[0] * 1000.0,
          1000.0 * data_next[1] / static_cast<double>(data_samplecount),
          data_next[2] * 1000.0,
          1000.0 * static_cast<double>(data_samplecount) /
              static_cast<double>(duration.count())};
      if (data.size() < data.capacity() - 1) {
        data.push_back(std::move(val));
        ++data_offset;
      } else {
        data[data_offset] = std::move(val);
        data_offset       = (data_offset + 1) % data.capacity();
      }
    }

    data_next[0]     = 1.0;
    data_next[1]     = 0.0;
    data_next[2]     = 0.0;
    data_samplecount = 0;
    last_update      = now;
  }

  data_next[0] = (t > data_next[0]) ? data_next[0] : t;
  data_next[1] += t;
  data_next[2] = (t > data_next[2]) ? t : data_next[2];
  ++data_samplecount;
}

void astri::debug::LoggingWindow::render() {}
