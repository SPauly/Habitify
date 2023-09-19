#include "src/frontend/ping/ping_gui.h"

#include <imgui.h>

#include "src/core/event_bus/event.h"
#include "src/core/event_bus/event_bus.h"

namespace habitify_frontend {
void PingGui::OnUIRender() {
  ImGui::Begin("Ping Service");
  if (ImGui::Button("Ping")) {
    ImGui::Text("Sending Ping");
    habitify_core::Event<int> event(habitify_core::EventType::TEST, 0, nullptr);
    auto p = habitify_core::EventBus::get_instance()->RequestPublishing(
        1, std::make_shared<habitify_core::Event<int>>(std::move(event)));
  }
  ImGui::End();
}
}  // namespace habitify_frontend
