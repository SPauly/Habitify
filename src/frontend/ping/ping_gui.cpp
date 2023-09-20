#include "src/frontend/ping/ping_gui.h"

#include <imgui.h>

#include "src/core/event_bus/event.h"
#include "src/core/event_bus/event_bus.h"

namespace habitify_frontend {
void PingGui::OnUIRender() {
  ImGui::Begin("Ping Service");
  static std::string ping_str = "Ping Received: ";
  static int ping_counter = 0;
  if (ImGui::Button("Ping")) {
    ImGui::Text("Sending Ping");

    ping_counter++;
    habitify_core::Event<int> event(habitify_core::EventType::TEST, 1,
                                    &ping_counter);
    auto p =
        habitify_core::EventBus::get_instance()->RequestPublishing(1, event);
    habitify_core::Listener l;
    l.SubscribeTo(0);
    if (l.HasNews()) {
      ping_str = "Ping Received: " + *l.ReadLatest<int>()->GetData<int>();
    }
  }
  ImGui::Text(ping_str.c_str());
  ImGui::End();
}
}  // namespace habitify_frontend
