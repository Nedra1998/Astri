#include "ui/windows/list.hpp"

#include <imgui.h>

void astri::ui::ListWindow::draw() {
  ImGui::Text("Satelite List");

  // ImGui Table with these columns: color, name, id
  ImGui::BeginTable("satelite_list", 3,
                    ImGuiTableFlags_Borders | ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg |
                        ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
                        ImGuiTableFlags_Hideable);
  ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 25.0f);
  ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50.0f);
  ImGui::TableSetupColumn("Name",
                          ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_DefaultSort);
  ImGui::TableHeadersRow();

  ImGui::TableNextRow();

  ImGui::TableNextColumn();
  static float col1[3] = {1.0f, 0.0f, 0.0f};
  ImGui::ColorEdit3("##color", col1, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

  ImGui::TableNextColumn();
  ImGui::Text("Starlink");

  ImGui::TableNextColumn();
  ImGui::Text("123456");

  ImGui::EndTable();
}
