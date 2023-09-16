#ifndef HABITIFY_SRC_CORE_APPLICATION_H_
#define HABITIFY_SRC_CORE_APPLICATION_H_

#include "src/core/imgui_frontend.h"

namespace habitify_core {
class Application {
 public:
  Application() = default;
  ~Application() = default;

  void Run();

 private:
};
}  // namespace habitify_core

#endif  // HABITIFY_SRC_CORE_APPLICATION_H_