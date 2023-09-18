#ifndef HABITIFY_SRC_FRONTEND_LAYER_STACK_H_
#define HABITIFY_SRC_FRONTEND_LAYER_STACK_H_

#include <memory>
#include <vector>

#include "src/frontend/layer.h"

namespace habitify_frontend {

class LayerStack {
 public:
  LayerStack() = default;
  ~LayerStack();

  template <typename T>
  void PushLayer() {
    static_assert(std::is_base_of<Layer, T>::value,
                  "Pushed type is not subclass of Layer!");
    layers_.emplace(layers_.begin() + layer_insert_index_,
                    std::make_shared<T>());
    (*std::next(layers_.begin(), layer_insert_index_))->OnAttach();
    layer_insert_index_++;
  }

  void PushLayer(const std::shared_ptr<Layer> &layer);
  void PushOverlay(const std::shared_ptr<Layer> &overlay);
  void PopLayer(std::shared_ptr<Layer> layer);
  void PopOverlay(std::shared_ptr<Layer> overlay);
  void HideLayer(std::shared_ptr<Layer> layer);
  void ShowLayer(std::shared_ptr<Layer> layer);

  void clear();

  std::vector<std::shared_ptr<Layer>>::iterator begin() {
    return layers_.begin();
  }
  std::vector<std::shared_ptr<Layer>>::iterator end() { return layers_.end(); }
  std::vector<std::shared_ptr<Layer>>::reverse_iterator rbegin() {
    return layers_.rbegin();
  }
  std::vector<std::shared_ptr<Layer>>::reverse_iterator rend() {
    return layers_.rend();
  }

  std::vector<std::shared_ptr<Layer>>::const_iterator begin() const {
    return layers_.begin();
  }
  std::vector<std::shared_ptr<Layer>>::const_iterator end() const {
    return layers_.end();
  }
  std::vector<std::shared_ptr<Layer>>::const_reverse_iterator rbegin() const {
    return layers_.rbegin();
  }
  std::vector<std::shared_ptr<Layer>>::const_reverse_iterator rend() const {
    return layers_.rend();
  }

 private:
  std::vector<std::shared_ptr<Layer>> layers_;
  std::vector<std::shared_ptr<Layer>> hidden_layers_;
  unsigned int layer_insert_index_ = 0;
};
}  // namespace habitify_frontend

#endif  // HABITIFY_SRC_FRONTEND_LAYER_STACK_H_