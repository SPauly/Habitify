#ifndef HABITIFY_SRC_CORE_LAYER_H
#define HABITIFY_SRC_CORE_LAYER_H

namespace habitify {
class Layer {
 public:
  virtual ~Layer() = default;

  virtual void OnAttach(){};
  virtual void OnDetach(){};

  virtual void OnUIRender(){};
};
}  // namespace habitify

#endif  // HABITIFY_SRC_CORE_LAYER_H