#include "src/frontend/imgui_frontend.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "src/frontend/debug_gui/debug_gui.h"
#include "src/frontend/ping/ping_gui.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && \
    !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

namespace habitify_frontend {
ImGuiFrontend::ImGuiFrontend() {}
ImGuiFrontend::~ImGuiFrontend() {}

bool ImGuiFrontend::Init() {
  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) return is_initialized = false;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char *glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

  // Create window with graphics context
  window_ = glfwCreateWindow(display_w_, display_h_, "Habitify", NULL, NULL);
  if (window_ == NULL) return false;
  glfwMakeContextCurrent(window_);
  glfwSwapInterval(1);  // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  io_ = &ImGui::GetIO();
  (void)io_;
  io_->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io_->ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
  io_->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  viewport_ = ImGui::GetMainViewport();

  // instantiate layers
  layer_stack_.PushLayer<habitify_debug::DebugGui>();
  layer_stack_.PushLayer<PingGui>();

  return is_initialized = true;
}

void ImGuiFrontend::Shutdown() {
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window_);
  glfwTerminate();

  is_initialized = false;
}

void ImGuiFrontend::Run() {
  if (!Init()) return;
  while (!glfwWindowShouldClose(window_)) {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
    // tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
    // your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
    // data to your main application, or clear/overwrite your copy of the
    // keyboard data. Generally you may always pass all inputs to dear imgui,
    // and hide them from your application based on those two flags.
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    for (auto &layer : layer_stack_) {
      layer->OnUIRender();
    }

    // Rendering
    ImGui::Render();
    glfwGetFramebufferSize(window_, &temp_display_w_, &temp_display_h_);
    glViewport(0, 0, temp_display_w_, temp_display_h_);
    glClearColor(clear_color_.x * clear_color_.w,
                 clear_color_.y * clear_color_.w,
                 clear_color_.z * clear_color_.w, clear_color_.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we
    // save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call
    //  glfwMakeContextCurrent(window) directly)
    if (io_->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow *backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(window_);
  }
  Shutdown();
}

}  // namespace habitify_frontend
