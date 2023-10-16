#ifndef HABITIFY_SRC_FRONTEND_IMGUI_FRONTEND_H_
#define HABITIFY_SRC_FRONTEND_IMGUI_FRONTEND_H_

#include <imgui.h>

#include <memory>

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>  // Will drag system OpenGL headers

#include "src/core/event_bus/event_bus.h"
#include "src/frontend/layer_stack.h"
namespace habitify_frontend {

class ImGuiFrontend {
 public:
  ImGuiFrontend() = delete;
  ImGuiFrontend(std::shared_ptr<::habitify_core::EventBus> event_bus);
  ~ImGuiFrontend();

  void Run();

 private:
  bool Init();
  void Shutdown();

 private:
  // flags
  bool is_initialized = false;

  // ImGui Frontend utils
  GLFWwindow *window_;
  ImGuiViewport *viewport_;
  ImGuiIO *io_;

  // Window size
  const int display_w_ = 445;
  const int display_h_ = 650;
  const int display_w_offset_graph_ = 445;
  int temp_display_w_, temp_display_h_;  // for temporary use

  // Style
  ImVec4 clear_color_ = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  ImVec4 *colors_ = nullptr;
  ImGuiStyle *style_ = nullptr;

  LayerStack layer_stack_;
  std::shared_ptr<::habitify_core::EventBus> event_bus_;
};

}  // namespace habitify_frontend

#endif  // HABITIFY_SRC_FRONTEND_IMGUI_FRONTEND_H_