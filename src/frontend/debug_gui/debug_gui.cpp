#include "src/frontend/debug_gui/debug_gui.h"

#include <imgui.h>

namespace habitify_debug {

void DebugGui::OnUIRender() {
  ImGui::Begin("DebugGui");
  ImGui::Text("Window width: %.1f Window Height: %.1f", ImGui::GetWindowWidth(),
              ImGui::GetWindowHeight());
  ImGui::End();
}

}  // namespace habitify_debug
