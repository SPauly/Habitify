#ifndef HABITIFY_SRC_CORE_APPLICATION_H_
#define HABITIFY_SRC_CORE_APPLICATION_H_

#include <thread>
#include <vector>

#include "src/core/imgui_frontend.h"

namespace habitify_core {
class Application {
 public:
  Application();
  ~Application();

  void Run();

 private:
  ImGuiFrontend imgui_frontend_;
  std::thread *frontend_thread_;
};
}  // namespace habitify_core

#endif  // HABITIFY_SRC_CORE_APPLICATION_H_