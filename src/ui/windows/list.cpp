#include "ui/windows/list.hpp"

#include <imgui.h>

#include "core/model.hpp"
#include "ecs.hpp"

void astri::ui::ListWindow::draw() {
  ImGui::BeginTable("satelite_list", 3,
                    ImGuiTableFlags_Borders | ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg |
                        ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
                        ImGuiTableFlags_Hideable);
  ImGui::TableSetupColumn(" ", ImGuiTableColumnFlags_WidthFixed, 25.0f);
  ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50.0f);
  ImGui::TableSetupColumn("Name",
                          ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_DefaultSort);
  ImGui::TableHeadersRow();

  ecs::WORLD.query<GeneralPertubations>().each([](GeneralPertubations &gp) {
    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    ImGui::Checkbox(fmt::format("##checkbox_{}", gp.object_id).c_str(), &gp.visible);

    ImGui::TableNextColumn();
    ImGui::Selectable(gp.object_id.c_str(), false, ImGuiSelectableFlags_SpanAllColumns);

    ImGui::TableNextColumn();
    ImGui::TextUnformatted(gp.object_name.c_str());
  });

  ImGui::EndTable();
}
