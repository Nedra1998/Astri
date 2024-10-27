#include "ui//windows/list.hpp"

#include <imgui.h>

#include "core/model.hpp"

bool fuzzy_match(const std::string &query, const std::string &target) {
  size_t query_idx = 0;
  for (size_t target_idx = 0; target_idx < target.size(); ++target_idx) {
    if (std::tolower(query[query_idx]) == std::tolower(target[target_idx])) {
      ++query_idx;
      if (query_idx == query.size()) {
        return true;
      }
    }
  }
  return false;
}

void astri::ui::ListWindow::draw() {
  LOG_ZONE();

  const auto &datastore = astri::core::Datastore::get();

  static char filter[256] = "";
  static int sort_column = 0;   // Keep track of which column is sorted
  static bool ascending = true; // Keep track of sorting direction

  ImGui::InputText("Filter", filter, sizeof(filter));
  ImGui::Text("Satellites: %lu", datastore.satellites.size());

  std::vector<core::GeneralPertubations> filtered;
  for (const auto &satellite : datastore.satellites) {
    if (filter[0] == '\0' || fuzzy_match(filter, satellite.object_name) ||
        fuzzy_match(filter, satellite.object_id)) {
      filtered.push_back(satellite);
    }
  }

  if (ImGui::BeginTable("Satellites", 6,
                        ImGuiTableFlags_Sortable | ImGuiTableFlags_Resizable |
                            ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
    ImGui::TableSetupColumn("Object Name", ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn("Object ID");
    ImGui::TableSetupColumn("Epoch");
    ImGui::TableSetupColumn("Mean Motion", ImGuiTableColumnFlags_PreferSortDescending);
    ImGui::TableSetupColumn("Eccentricity");
    ImGui::TableSetupColumn("Inclination");
    ImGui::TableHeadersRow();

    ImGuiTableSortSpecs *sort_specs = ImGui::TableGetSortSpecs();
    if (sort_specs && sort_specs->SpecsDirty) {
      sort_column = sort_specs->Specs->ColumnIndex;
      ascending = (sort_specs->Specs->SortDirection == ImGuiSortDirection_Ascending);
      sort_specs->SpecsDirty = false;
    }

    // Perform sorting based on the current sort column and direction
    std::sort(filtered.begin(), filtered.end(),
              [&](const core::GeneralPertubations &a, const core::GeneralPertubations &b) {
                switch (sort_column) {
                case 0:
                  return ascending ? a.object_name < b.object_name : a.object_name > b.object_name;
                case 1:
                  return ascending ? a.object_id < b.object_id : a.object_id > b.object_id;
                case 2:
                  return ascending ? a.epoch < b.epoch : a.epoch > b.epoch;
                case 3:
                  return ascending ? a.mean_motion < b.mean_motion : a.mean_motion > b.mean_motion;
                case 4:
                  return ascending ? a.eccentricity < b.eccentricity
                                   : a.eccentricity > b.eccentricity;
                case 5:
                  return ascending ? a.inclination < b.inclination : a.inclination > b.inclination;
                default:
                  return false;
                }
              });

    for (const auto &item : filtered) {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("%s", item.object_name.c_str());
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%s", item.object_id.c_str());
      ImGui::TableSetColumnIndex(2);
      ImGui::Text("%s", fmt::format("{:%Y-%m-%dT%H:%M:%S}", item.epoch).c_str());
      ImGui::TableSetColumnIndex(3);
      ImGui::Text("%.6f", item.mean_motion);
      ImGui::TableSetColumnIndex(4);
      ImGui::Text("%.6f", item.eccentricity);
      ImGui::TableSetColumnIndex(5);
      ImGui::Text("%.6f", item.inclination);
    }

    ImGui::EndTable();
  }

  // for (const auto &satellite : datastore.satellites) {
  //   ImGui::Text("%s", satellite.object_name.c_str());
  // }

  ImGui::Text("Hello World!");
}
