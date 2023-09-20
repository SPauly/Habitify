#include "src/core/event_bus/event_bus.h"

#include <chrono>

namespace habitify_core {
EventBus::~EventBus() {}

std::shared_ptr<Publisher> EventBus::RequestPublishing(
    const ChannelIdType& channel, std::shared_ptr<EventBase> event,
    bool need_response) {
  std::unique_lock lock(mux_channels_);

  std::shared_ptr<Publisher> publisher =
      std::make_shared<Publisher>(channel, event);
  channels_.emplace(std::make_pair(channel, publisher));

  return publisher;
}

Publisher::Publisher(const ChannelIdType& channel, const EventBase& event)
    : channel_id_(channel) {
  this->Publish(event);
}

const ChannelIdType& Publisher::Publish(const EventBase& event) {
  std::unique_lock lock(mux_events_);

  event_map_.emplace(std::make_pair(
      map_index_, std::make_shared<EventBase>(std::move(event))));
  map_index_++;

  cv_->notify_all();

  return response_channel_id_;
}

const std::shared_ptr<const EventBase> Publisher::ReadLatest() const {
  std::shared_lock lock(mux_events_);
  return event_map_.at(map_index_ - 1);
}

bool Publisher::HasNext(size_t index) {
  if (index >= map_index_) return false;

  // check for deleted events here
  return true;
}

bool Publisher::Wait(int ms, size_t index) {
  if (HasNext(index)) return true;

  std::unique_lock<std::shared_mutex> lock(mux_events_);

  auto predicate = [this, index]() { return HasNext(index); };

  if (ms == 0)
    cv_->wait(lock, predicate);
  else if (!cv_->wait_for(lock, std::chrono::milliseconds(ms), predicate))
    return false;

  return true;
}

Listener::Listener(const ChannelIdType& channel) : channel_id_(channel) {}

}  // namespace habitify_core
