#include "src/frontend/ping/ping_gui.h"

#include <imgui.h>

#include <memory>
#include <string>

#include "src/core/event_bus/event.h"
#include "src/core/event_bus/event_bus.h"

namespace habitify_frontend {
void PingGui::OnUIRender() {
  ImGui::Begin("Ping Service");

  static std::string ping_str;
  static habitify_core::Listener l(0);

  if (l.HasNews())
    ping_str =
        "Ping Received: " + std::to_string(*l.ReadLatest()->GetData<int>());

  ImGui::Text(ping_str.c_str());
  ImGui::End();
}
}  // namespace habitify_frontend
