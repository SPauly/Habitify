#ifndef HABITIFY_SRC_FRONTEND_DEBUG_GUI_DEBUG_GUI_H_
#define HABITIFY_SRC_FRONTEND_DEBUG_GUI_DEBUG_GUI_H_

#include "src/frontend/layer.h"

namespace habitify_debug {
class DebugGui : public habitify_frontend::Layer {
 public:
  void OnUIRender() override;
};

}  // namespace habitify_debug

#endif  // HABITIFY_SRC_FRONTEND_DEBUG_GUI_DEBUG_GUI_H_