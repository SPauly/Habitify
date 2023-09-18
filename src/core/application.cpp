#include "src/core/application.h"

#include <iostream>

#include "src/core/error_handler.h"

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

void Application::Run() { std::cin.get(); }

}  // namespace habitify_core