#ifndef HABITIFY_SRC_CORE_APPLICATION_H_
#define HABITIFY_SRC_CORE_APPLICATION_H_

#include <thread>
#include <vector>

#include "src/frontend/imgui_frontend.h"

namespace habitify_core {
class Application {
 public:
  Application();
  ~Application();

  void Run();

 private:
  habitify_frontend::ImGuiFrontend imgui_frontend_;
  std::thread *frontend_thread_;

  std::shared_ptr<EventBus> event_bus_;
};
}  // namespace habitify_core

#endif  // HABITIFY_SRC_CORE_APPLICATION_H_