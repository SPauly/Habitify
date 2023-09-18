#include "src/ping/ping_gui.h"

#include <imgui.h>

namespace habitify_frontend {
void PingGui::OnUIRender() {
  ImGui::Begin("Ping Service");
  if (ImGui::Button("Ping")) ImGui::Text("Sending Ping");
  ImGui::End();
}
}  // namespace habitify_frontend
