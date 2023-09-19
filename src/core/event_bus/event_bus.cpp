#include "src/core/event_bus/event_bus.h"

namespace habitify_core {
EventBus::~EventBus() {}

std::shared_ptr<Publisher> EventBus::RequestPublishing(
    const ChannelIdType& channel, std::shared_ptr<EventBase> event,
    bool need_response) {
  std::lock_guard<std::mutex> lock(mux_channels_);

  std::shared_ptr<Publisher> publisher =
      std::make_shared<Publisher>(channel, event);
  channels_.emplace(std::make_pair(channel, publisher));

  return publisher;
}

Publisher::Publisher(const ChannelIdType& channel,
                     std::shared_ptr<EventBase> event)
    : channel_id_(channel) {
  this->Publish(event);
}

const ChannelIdType& Publisher::Publish(std::shared_ptr<EventBase> event) {
  std::lock_guard<std::mutex> lock(mux_events_);

  event_map_.emplace(std::make_pair(map_index_, event));
  map_index_++;

  return response_channel_id_;
}

}  // namespace habitify_core
