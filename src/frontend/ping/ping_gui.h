#ifndef HABITIFY_SRC_FRONTEND_PING_PING_GUI_H_
#define HABITIFY_SRC_FRONTEND_PING_PING_GUI_H_

#include <memory>

#include "src/core/event_bus/event_bus.h"
#include "src/frontend/layer.h"

namespace habitify_frontend {
class PingGui : public Layer {
 public:
  PingGui();
  ~PingGui() = default;
  void OnUIRender() override;

 private:
  std::shared_ptr<::habitify_core::Listener> listener_;
  std::shared_ptr<::habitify_core::Publisher<int>> publisher_;
};
}  // namespace habitify_frontend

#endif  // HABITIFY_SRC_FRONTEND_PING_PING_GUI_H_