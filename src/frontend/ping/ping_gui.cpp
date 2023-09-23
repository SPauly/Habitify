#include "src/frontend/ping/ping_gui.h"

#include <imgui.h>

#include <memory>
#include <string>

#include "src/core/event_bus/event.h"
#include "src/core/event_bus/event_bus.h"

namespace habitify_frontend {
PingGui::PingGui() {
  listener_ = habitify_core::Listener::Create();
  listener_->SubscribeTo(0);
}
void PingGui::OnUIRender() {
  ImGui::Begin("Ping Service");

  static std::string ping_str;

  if (listener_->HasNews())
    ping_str = "Ping Received: " +
               std::to_string(*listener_->ReadLatest<int>()->GetData<int>());

  ImGui::Text(ping_str.c_str());
  ImGui::End();
}
}  // namespace habitify_frontend
