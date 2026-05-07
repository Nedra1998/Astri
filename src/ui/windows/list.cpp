#include "ui/windows/list.hpp"

#include <imgui.h>
#include <sgp4/element_set.hpp>

#include "ecs.hpp"

enum Columns : int {
  COL_VISIBLE = 0,
  // COL_COLOR,
  COL_ID,
  COL_NAME,
  COL_GROUP,
  COL_INCL,
  COL_ALT,
  COL_COUNT
};

static float mean_altitude(const sgp4::ElementSet &es) {
  if (es.mean_motion <= 0.0f) {
    return 0.0f; // Invalid mean motion
  }
  const float n = es.mean_motion * 2.0f * M_PI / 86400.0f; // Convert rev/day to rad/s
  const float a = std::cbrt(es.econst.mus / (n * n));      // Semi-major axis in km
  return a - es.econst.radus_km; // Subtract Earth's radius to get altitude
}

void astri::ui::ListWindow::draw() {
  ImGui::BeginTable("satelite_list", COL_COUNT,
                    ImGuiTableFlags_Borders | ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg |
                        ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
                        ImGuiTableFlags_Hideable | ImGuiTableFlags_ScrollY |
                        ImGuiTableFlags_SizingFixedFit);

  ImGui::TableSetupColumn(" ", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort,
                          24.0f, COL_VISIBLE);
  ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 60.0f, COL_ID);
  ImGui::TableSetupColumn(
      "Name", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_DefaultSort, 0, COL_ID);
  ImGui::TableSetupColumn("Group", ImGuiTableColumnFlags_WidthFixed, 110.0f, COL_GROUP);
  ImGui::TableSetupColumn("Incl (°)", ImGuiTableColumnFlags_WidthFixed, 65.0f, COL_GROUP);
  ImGui::TableSetupColumn("Alt (km)", ImGuiTableColumnFlags_WidthFixed, 65.0f, COL_GROUP);
  ImGui::TableHeadersRow();

  if (ImGuiTableSortSpecs *sort_specs = ImGui::TableGetSortSpecs()) {
    if (sort_specs->SpecsDirty || cache_dirty_) {
      // Rebuild entity list from ECS
      sorted_satelites_.clear();
      ecs::WORLD.query<sgp4::ElementSet, SatelliteState>().each(
          [&](sgp4::ElementSet &gp, SatelliteState &state) {
            sorted_satelites_.emplace_back(gp, state);
          });

      if (sort_specs->SpecsCount > 0) {
        const ImGuiTableColumnSortSpecs &spec = sort_specs->Specs[0];
        const bool asc = spec.SortDirection == ImGuiSortDirection_Ascending;

        std::sort(sorted_satelites_.begin(), sorted_satelites_.end(),
                  [&](const std::tuple<sgp4::ElementSet, SatelliteState> &a,
                      const std::tuple<sgp4::ElementSet, SatelliteState> &b) {
                    const sgp4::ElementSet &gp_a = std::get<0>(a);
                    const sgp4::ElementSet &gp_b = std::get<0>(b);
                    const SatelliteState &state_a = std::get<1>(a);
                    const SatelliteState &state_b = std::get<1>(b);

                    int cmp = 0;
                    switch (spec.ColumnUserID) {
                    case COL_ID:
                      cmp = gp_a.object_id.compare(gp_b.object_id);
                      break;
                    case COL_NAME:
                      cmp = gp_a.object_name.compare(gp_b.object_name);
                      break;
                    case COL_GROUP:
                      cmp = state_a.group.compare(state_b.group);
                      break;
                    case COL_INCL:
                      cmp = (gp_a.inclination > gp_b.inclination) ? 1 : -1;
                    case COL_ALT:
                      cmp = (gp_a.mean_motion < gp_b.mean_motion) ? 1 : -1;
                    default:
                      cmp = gp_a.object_name.compare(gp_b.object_name);
                    }

                    return asc ? (cmp < 0) : (cmp > 0);
                  });
      }

      sort_specs->SpecsDirty = false;
      cache_dirty_ = false;
    }
  }

  for (const auto &entity : sorted_satelites_) {
    ImGui::TableNextRow();
    const sgp4::ElementSet &gp = std::get<0>(entity);
    const SatelliteState &state = std::get<1>(entity);

    flecs::entity e = ecs::WORLD.entity(state.entity_id);
    SatelliteState *ss = e.get_mut<SatelliteState>();

    ImGui::PushID(state.entity_id);

    ImGui::TableSetColumnIndex(COL_VISIBLE);
    ImGui::Checkbox("##vis", &ss->visible);

    ImGui::TableSetColumnIndex(COL_ID);
    ImGui::TextUnformatted(gp.object_id.c_str());

    ImGui::TableSetColumnIndex(COL_NAME);
    ImGui::TextUnformatted(gp.object_name.c_str());

    ImGui::TableSetColumnIndex(COL_GROUP);
    ImGui::TextUnformatted(state.group.c_str());

    ImGui::TableSetColumnIndex(COL_INCL);
    ImGui::Text("%.2f", gp.inclination);

    ImGui::TableSetColumnIndex(COL_ALT);
    ImGui::Text("%.0f", mean_altitude(gp));

    ImGui::PopID();
  }

  ImGui::EndTable();
}
