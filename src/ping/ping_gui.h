#ifndef HABITIFY_SRC_PING_PING_GUI_H_
#define HABITIFY_SRC_PING_PING_GUI_H_

#include "src/frontend/layer.h"

namespace habitify_frontend {
class PingGui : public Layer {
 public:
  void OnUIRender() override;
};
}  // namespace habitify_frontend

#endif  // HABITIFY_SRC_PING_PING_GUI_H_