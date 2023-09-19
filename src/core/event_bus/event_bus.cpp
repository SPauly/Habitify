#include "src/core/event_bus/event_bus.h"

namespace habitify_core {
EventBus::~EventBus() {}

std::shared_ptr<Publisher> EventBus::RequestPublishing(
    const ChannelIdType& channel, std::shared_ptr<EventBase> data,
    bool need_response) {
  std::lock_guard<std::mutex> lock(mux_channels_);

  std::shared_ptr<Publisher> publisher =
      std::make_shared<Publisher>(channel, data);
  channels_.emplace(std::make_pair(channel, publisher));

  return publisher;
}

Publisher::Publisher(const ChannelIdType& channel,
                     std::shared_ptr<EventBase> data)
    : channel_id_(channel) {
  this->Publish(data);
}

const ChannelIdType& Publisher::Publish(std::shared_ptr<EventBase> event) {
  std::lock_guard<std::mutex> lock(mux_data_);

  data_que_.push_back(event);

  return response_channel_id_;
}

}  // namespace habitify_core
