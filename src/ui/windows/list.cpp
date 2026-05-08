#include "ui/windows/list.hpp"

#include <algorithm>
#include <map>

#include <imgui.h>
#include <imgui_internal.h>
#include <sgp4/element_set.hpp>

#include "ecs.hpp"
#include "state.hpp"

enum Columns : int { COL_VISIBLE = 0, COL_ID, COL_NAME, COL_INCL, COL_ALT, COL_COUNT };

static float mean_altitude(const sgp4::ElementSet &es) {
  if (es.mean_motion <= 0.0f) {
    return 0.0f; // Invalid mean motion
  }
  const float n = es.mean_motion * 2.0f * M_PI / 86400.0f; // Convert rev/day to rad/s
  const float a = std::cbrt(es.econst.mus / (n * n));      // Semi-major axis in km
  return a - es.econst.radus_km; // Subtract Earth's radius to get altitude
}

static bool matches_filter(const std::string &search, const sgp4::ElementSet &gp,
                           const astri::SatelliteState &ss) {
  if (search.empty())
    return true;

  const auto ci_find = [&](const std::string &str) {
    auto it = std::search(str.begin(), str.end(), search.begin(), search.end(), [](char a, char b) {
      return std::tolower((unsigned char)a) == std::tolower((unsigned char)b);
    });
    return it != str.end();
  };

  if (ci_find(gp.object_name))
    return true;
  if (ci_find(gp.object_id))
    return true;
  if (ci_find(ss.group))
    return true;
  return false;
}

void astri::ui::ListWindow::draw() {

  // if (ImGui::Checkbox("##grpvis", &group_vis)) {
  //   for (std::size_t idx : indices) {
  //     const SatelliteState &state = std::get<1>(sorted_satelites_[idx]);
  //     flecs::entity e = ecs::WORLD.entity(state.entity_id);
  //     SatelliteState *ss = e.get_mut<SatelliteState>();
  //     if (ss) {
  //       ss->visible = group_vis;
  //     }
  //   }
  // }
  if (ImGui::Button("  Toggle All")) {
    for (const auto &sat : sorted_satelites_) {
      flecs::entity e = ecs::WORLD.entity(sat.second.entity_id);
      SatelliteState *ss = e.get_mut<SatelliteState>();
      if (ss) {
        ss->visible = !ss->visible;
      }
    }
  }
  ImGui::SameLine();
  const bool search_changed = ImGui::InputTextWithHint("##search", "Search by name or ID...",
                                                       search_buffer_, sizeof(search_buffer_));
  if (search_changed || std::string(search_buffer_) != search_string_) {
    search_string_ = search_buffer_;
    cache_dirty_ = true;
  }

  ImGui::Spacing();

  ImGui::BeginTable("satelite_list", COL_COUNT,
                    ImGuiTableFlags_Borders | ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg |
                        ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
                        ImGuiTableFlags_Hideable | ImGuiTableFlags_ScrollY |
                        ImGuiTableFlags_SizingFixedFit);

  ImGui::TableSetupColumn(" ", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort,
                          24.0f, COL_VISIBLE);
  ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 60.0f, COL_ID);
  ImGui::TableSetupColumn(
      "Name", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_DefaultSort, 0, COL_NAME);
  ImGui::TableSetupColumn("Incl (°)", ImGuiTableColumnFlags_WidthFixed, 65.0f, COL_INCL);
  ImGui::TableSetupColumn("Alt (km)", ImGuiTableColumnFlags_WidthFixed, 65.0f, COL_ALT);
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
                  [&](const std::pair<sgp4::ElementSet, SatelliteState> &a,
                      const std::pair<sgp4::ElementSet, SatelliteState> &b) {
                    const sgp4::ElementSet &gp_a = a.first;
                    const sgp4::ElementSet &gp_b = b.first;
                    const SatelliteState &state_a = a.second;
                    const SatelliteState &state_b = b.second;

                    int cmp = 0;
                    switch (spec.ColumnUserID) {
                    case COL_ID:
                      cmp = gp_a.object_id.compare(gp_b.object_id);
                      break;
                    case COL_NAME:
                      cmp = gp_a.object_name.compare(gp_b.object_name);
                      break;
                    case COL_INCL:
                      cmp = (gp_a.inclination > gp_b.inclination) ? 1 : -1;
                      break;
                    case COL_ALT:
                      cmp = (gp_a.mean_motion < gp_b.mean_motion) ? 1 : -1;
                      break;
                    default:
                      cmp = gp_a.object_name.compare(gp_b.object_name);
                    }

                    return asc ? (cmp < 0) : (cmp > 0);
                  });
      }

      // Build group cache, for grouping the satellites by their group name
      group_cache_.clear();
      group_order_.clear();
      for (std::size_t i = 0; i < sorted_satelites_.size(); ++i) {
        const SatelliteState &state = sorted_satelites_[i].second;
        if (group_cache_.find(state.group) == group_cache_.end()) {
          group_order_.push_back(state.group);
          group_cache_[state.group] = std::make_pair(false, std::vector<std::size_t>{i});
        } else {
          group_cache_[state.group].second.push_back(i);
        }
      }

      sort_specs->SpecsDirty = false;
      cache_dirty_ = false;
    }
  }

  for (const std::string &group_name : group_order_) {
    const auto &indices = group_cache_[group_name].second;

    if (group_name.empty() || indices.size() == 1) {
      // Render ungrouped satellites or groups with a single satellite directly
      // without a tree node
      for (std::size_t idx : indices) {
        const sgp4::ElementSet &gp = sorted_satelites_[idx].first;
        const SatelliteState &state = sorted_satelites_[idx].second;
        if (!matches_filter(search_string_, gp, state)) {
          continue;
        }

        flecs::entity e = ecs::WORLD.entity(state.entity_id);
        SatelliteState *ss = e.get_mut<SatelliteState>();

        ImGui::TableNextRow();
        ImGui::PushID(static_cast<int>(state.entity_id));

        ImGui::TableSetColumnIndex(COL_VISIBLE);
        ImGui::Checkbox("##vis", &ss->visible);

        ImGui::TableSetColumnIndex(COL_ID);
        ImGui::TextUnformatted(gp.object_id.c_str());

        ImGui::TableSetColumnIndex(COL_NAME);
        ImGui::TreeNodeEx(gp.object_name.c_str(), ImGuiTreeNodeFlags_Leaf |
                                                      ImGuiTreeNodeFlags_NoTreePushOnOpen |
                                                      ImGuiTreeNodeFlags_SpanAllColumns);

        ImGui::TableSetColumnIndex(COL_INCL);
        ImGui::Text("%.2f", gp.inclination);

        ImGui::TableSetColumnIndex(COL_ALT);
        ImGui::Text("%.0f", mean_altitude(gp));

        ImGui::PopID();
      }
    } else {
      // Render a tree node for the group with all the satellites contained
      // within that group node

      // Check if any satellites in the group are visible, and if all are
      // visible, to determine the state of the group checkbox
      bool any_visible = false;
      bool all_visible = true;
      bool all_filtered = true;
      for (std::size_t idx : indices) {
        flecs::entity e = ecs::WORLD.entity(sorted_satelites_[idx].second.entity_id);
        const SatelliteState *ss = e.get<SatelliteState>();
        if (!all_filtered || matches_filter(search_string_, sorted_satelites_[idx].first,
                                            sorted_satelites_[idx].second)) {
          all_filtered = false;
        }
        if (ss && ss->visible) {
          any_visible = true;
        } else {
          all_visible = false;
        }
      }
      if (all_filtered) {
        continue; // Skip rendering the group if all satellites are filtered out
      }

      const bool mixed = any_visible && !all_visible;

      // Group header row
      ImGui::TableNextRow();
      ImGui::PushID(group_name.c_str());

      ImGui::TableSetColumnIndex(COL_VISIBLE);
      bool group_vis = all_visible;
      if (mixed) {
        ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, true);
      }
      if (ImGui::Checkbox("##grpvis", &group_vis)) {
        for (std::size_t idx : indices) {
          const SatelliteState &state = std::get<1>(sorted_satelites_[idx]);
          flecs::entity e = ecs::WORLD.entity(state.entity_id);
          SatelliteState *ss = e.get_mut<SatelliteState>();
          if (ss) {
            ss->visible = group_vis;
          }
        }
      }
      if (mixed) {
        ImGui::PopItemFlag();
      }

      ImGui::TableSetColumnIndex(COL_NAME);
      bool &open = group_cache_[group_name].first;
      ImGui::SetNextItemOpen(open, ImGuiCond_Once);
      open = ImGui::TreeNodeEx(group_name.c_str(), ImGuiTreeNodeFlags_SpanAllColumns, "%s (%zu)",
                               group_name.c_str(), indices.size());
      if (open) {
        ImGui::TreePop();
      }

      ImGui::PopID();

      if (!open) {
        continue;
      }

      // Child rows
      for (std::size_t idx : indices) {
        const sgp4::ElementSet &gp = sorted_satelites_[idx].first;
        const SatelliteState &state = sorted_satelites_[idx].second;
        if (!matches_filter(search_string_, gp, state)) {
          continue;
        }

        flecs::entity e = ecs::WORLD.entity(state.entity_id);
        SatelliteState *ss = e.get_mut<SatelliteState>();

        ImGui::TableNextRow();
        ImGui::PushID(static_cast<int>(state.entity_id));

        ImGui::TableSetColumnIndex(COL_VISIBLE);
        ImGui::Checkbox("##vis", &(ss->visible));

        ImGui::TableSetColumnIndex(COL_ID);
        ImGui::TextUnformatted(gp.object_id.c_str());

        ImGui::TableSetColumnIndex(COL_NAME);
        ImGui::TreeNodeEx(gp.object_name.c_str(), ImGuiTreeNodeFlags_Leaf |
                                                      ImGuiTreeNodeFlags_NoTreePushOnOpen |
                                                      ImGuiTreeNodeFlags_SpanAllColumns);

        ImGui::TableSetColumnIndex(COL_INCL);
        ImGui::Text("%.2f", gp.inclination);

        ImGui::TableSetColumnIndex(COL_ALT);
        ImGui::Text("%.0f", mean_altitude(gp));

        ImGui::PopID();
      }
    }
  }

  ImGui::EndTable();
}
