#include "src/core/application.h"

#include <iostream>

#include "src/core/error_handler.h"
#include "src/core/event_bus/event_bus.h"

namespace habitify_core {

Application::Application() {
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
  Listener l(1);
  std::shared_ptr<Publisher> p = std::make_shared<Publisher>(0);
  EventBus::get_instance()->RegisterPublisher(p);
  int ping_count = 0;

  Event<int> e(habitify_core::EventType::TEST, 0, &ping_count);

  while (!l.SubscribeTo(1)) {
  }
  while (l.Wait()) {
    auto latest = l.ReadLatest<int>();
    ping_count++;
    std::cout << "Incomming Ping Request No: " << *latest->GetData<int>()
              << std::endl;
    std::cout << "Respond with: " << ping_count << std::endl;
    p->Publish(e);
  }
  std::cin.get();
}

}  // namespace habitify_core