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
  static std::string ping_send_str;

  if (listener_->HasNews())
    ping_str = "Ping Received: " +
               std::to_string(*listener_->ReadLatest<int>()->GetData<int>());

  if (ImGui::Button("Send Ping")) {
    publisher_ = habitify_core::Publisher<int>::Create();
    publisher_->RegisterPublisher(1);
    static int ping_count = 0;
    habitify_core::Event<int> e(habitify_core::EventType::TEST, 0, &ping_count);
    ping_count++;
    ping_send_str = "Sending Ping: " + std::to_string(ping_count);
    publisher_->Publish(std::make_unique<const habitify_core::Event<int>>(e));
  }
  ImGui::Text(ping_send_str.c_str());
  ImGui::Text(ping_str.c_str());
  ImGui::End();
}
}  // namespace habitify_frontend
