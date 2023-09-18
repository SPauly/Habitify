#ifndef HABITIFY_SRC_FRONTEND_LAYER_H_
#define HABITIFY_SRC_FRONTEND_LAYER_H_

namespace habitify_frontend {
class Layer {
 public:
  virtual ~Layer() = default;

  virtual void OnAttach(){};
  virtual void OnDetach(){};

  virtual void OnUIRender(){};
};
}  // namespace habitify_frontend

#endif  // HABITIFY_SRC_FRONTEND_LAYER_H_