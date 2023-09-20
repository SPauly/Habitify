#ifndef HABITIFY_SRC_CORE_EVENT_BUS_EVENT_BUS_H_
#define HABITIFY_SRC_CORE_EVENT_BUS_EVENT_BUS_H_

#include <condition_variable>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

#include "src/core/event_bus/event.h"

namespace habitify_core {

class Publisher;
class Listener;

class EventBus {
 public:
  ~EventBus();

  // we use mutexes so this class needs to be noncopyable
  EventBus(const EventBus&) = delete;
  EventBus& operator=(const EventBus&) = delete;

  inline static std::shared_ptr<EventBus> get_instance() {
    static std::shared_ptr<EventBus> instance(new EventBus);
    return instance;
  }

  std::shared_ptr<Publisher> RequestPublishing(
      const ChannelIdType& channel, std::shared_ptr<EventBase> data = nullptr,
      bool need_response = false);

 private:
  EventBus() = default;

 private:
  mutable std::shared_mutex mux_channels_;

  std::unordered_map<ChannelIdType, std::shared_ptr<Publisher>> channels_;
  std::deque<std::shared_ptr<Publisher>> cache_;
};

class Publisher {
 public:
  friend class Listener;

  Publisher() = default;
  Publisher(const ChannelIdType& channel, const EventBase& event);
  ~Publisher() = default;

  Publisher(const Publisher&) = delete;
  const Publisher& operator=(const Publisher&) = delete;

  const ChannelIdType& Publish(const EventBase& event);

 private:
  const std::shared_ptr<const EventBase> ReadLatest() const;

  bool HasNext(size_t index);
  bool Wait(int ms, size_t index);

 private:
  mutable std::shared_mutex mux_events_;
  std::shared_ptr<std::condition_variable_any> cv_;

  size_t map_index_ = 0;
  std::unordered_map<int, std::shared_ptr<const EventBase>> event_map_;

  ChannelIdType channel_id_, response_channel_id_;
};

class Listener {
 public:
  Listener() = delete;
  Listener(const ChannelIdType&);
  ~Listener() = default;

  inline bool Wait(int ms) { return publisher_->Wait(ms, read_index_); }
  inline bool HasNews() { return publisher_->HasNext(read_index_); }
  void EnableCallback(std::function<void()> callback) {}

  template <typename T>
  const std::shared_ptr<const Event<T>> ReadLatest() const;

 private:
  const ChannelIdType channel_id_;
  std::shared_ptr<Publisher> publisher_;
  size_t read_index_ = 0;
};

template <typename T>
const std::shared_ptr<const Event<T>> Listener::ReadLatest() const {
  return static_cast<Event<T>>(publisher_->ReadLatest());
}

}  // namespace habitify_core

#endif  // HABITIFY_SRC_CORE_EVENT_BUS_EVENT_BUS_H_

/*
Usage:

Event<proto::PingMessage> e("hello");
Publisher p;
auto ret_channel = p.publish(e, true);
Listener l(ret_channel);
if(l.wait())
    std::string response = l.pull<std::string>();
*/