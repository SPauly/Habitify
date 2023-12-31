#include "src/core/application.h"

#include <iostream>

#include "src/core/error_handler.h"
#include "src/core/event_bus/event_bus.h"

namespace habitify_core {

Application::Application() : event_bus_(EventBus::Create()) {
  imgui_frontend_.SetEventBus(event_bus_);
  frontend_thread_ = new (std::nothrow)
      std::thread(&habitify_frontend::ImGuiFrontend::Run, &imgui_frontend_);
  HAB_ASSERT(frontend_thread_);
}

Application::~Application() {
  if (frontend_thread_) {
    frontend_thread_->join();
    delete frontend_thread_;
  }
}

void Application::Run() {
  std::shared_ptr<EventBus> event_bus = EventBus::Create();
  std::shared_ptr<Publisher<int>> p = event_bus_->RegisterPublisher<int>(0);
  std::shared_ptr<Listener> l = event_bus_->SubscribeTo(1);

  int ping_count = 0;

  Event<int> e(habitify_core::EventType::TEST, 0, &ping_count);

  while (std::cin.get()) {
    ping_count++;
    std::cout << "Sending Ping: " << ping_count << std::endl;
    p->Publish(std::make_unique<const Event<int>>(e));
    if (l->HasReceivedEvent())
      std::cout << "Ping Received: " << *l->ReadLatest<int>()->GetData<int>()
                << std::endl;
  }
}

}  // namespace habitify_core