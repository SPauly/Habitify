#ifndef HABITIFY_SRC_CORE_LAYER_H
#define HABITIFY_SRC_CORE_LAYER_H

namespace habitify_core {
class Layer {
 public:
  virtual ~Layer() = default;

  virtual void OnAttach(){};
  virtual void OnDetach(){};

  virtual void OnUIRender(){};
};
}  // namespace habitify_core

#endif  // HABITIFY_SRC_CORE_LAYER_H