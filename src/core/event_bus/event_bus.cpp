#include "src/core/event_bus/event_bus.h"

#include <chrono>

namespace habitify_core {
EventBus::~EventBus() {}

std::shared_ptr<Publisher> EventBus::RegisterPublisher(
    std::shared_ptr<Publisher> publisher) {
  if (GetPublisher(publisher->get_channel_id())) return publisher;

  std::unique_lock<std::shared_mutex> lock(mux_channels_);
  channels_.emplace(std::make_pair(publisher->get_channel_id(), publisher));

  return publisher;
}

std::shared_ptr<Publisher> EventBus::RequestPublishing(
    const ChannelIdType& channel, bool need_response) {
  std::unique_lock<std::shared_mutex> lock(mux_channels_);

  if (channels_.find(channel) != channels_.end()) return channels_.at(channel);

  std::shared_ptr<Publisher> publisher = std::make_shared<Publisher>(channel);
  channels_.emplace(std::make_pair(channel, publisher));

  return publisher;
}

std::shared_ptr<Publisher> EventBus::RequestPublishing(
    const ChannelIdType& channel, std::unique_ptr<const EventBase> event,
    bool need_response) {
  std::unique_lock<std::shared_mutex> lock(mux_channels_);

  if (channels_.find(channel) != channels_.end()) return channels_.at(channel);

  std::shared_ptr<Publisher> publisher =
      std::make_shared<Publisher>(channel, std::move(event));
  channels_.emplace(channel, publisher);

  return publisher;
}

std::shared_ptr<Publisher> EventBus::GetPublisher(
    const ChannelIdType& channel) {
  std::shared_lock<std::shared_mutex> lock(mux_channels_);
  if (channels_.find(channel) != channels_.end()) return channels_.at(channel);

  return nullptr;
}

Publisher::Publisher() : cv_(std::make_shared<std::condition_variable_any>()) {}

Publisher::Publisher(const ChannelIdType& channel)
    : channel_id_(channel),
      cv_(std::make_shared<std::condition_variable_any>()) {}

Publisher::Publisher(const ChannelIdType& channel,
                     std::unique_ptr<const EventBase> event)
    : Publisher(channel) {
  this->Publish(std::move(event));
}

void Publisher::Publish(std::unique_ptr<const EventBase> event) {
  std::unique_lock<std::shared_mutex> lock(mux_events_);

  event_map_.emplace(map_index_,
                     std::shared_ptr<const EventBase>(event.release()));
  cv_->notify_all();

  map_index_++;
}

const std::shared_ptr<const EventBase> Publisher::ReadLatest() const {
  std::shared_lock<std::shared_mutex> lock(mux_events_);
  return event_map_.at(map_index_ - 1);
}

bool Publisher::HasNext(size_t index) {
  std::shared_lock<std::shared_mutex> lock(mux_events_);
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

Listener::Listener() : event_bus_(EventBus::get_instance()) {}

Listener::Listener(const ChannelIdType& channel) : channel_id_(channel) {
  this->SubscribeTo(channel);
}

bool Listener::SubscribeTo(const ChannelIdType& channel) {
  channel_id_ = channel;
  read_index_ = 0;
  return (publisher_ = EventBus::get_instance()->GetPublisher(channel)) !=
         nullptr;
}

const std::shared_ptr<const EventBase> Listener::ReadLatest() {
  read_index_++;
  return ValidatePublisher() ? publisher_->ReadLatest() : nullptr;
}

}  // namespace habitify_core
