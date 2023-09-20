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

  std::shared_ptr<Publisher> RegisterPublisher(
      std::shared_ptr<Publisher> publisher);
  std::shared_ptr<Publisher> RequestPublishing(const ChannelIdType& channel,
                                               bool need_response = false);
  std::shared_ptr<Publisher> RequestPublishing(
      const ChannelIdType& channel, std::unique_ptr<const EventBase> event,
      bool need_response = false);

  std::shared_ptr<Publisher> GetPublisher(const ChannelIdType& channel);

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

  Publisher();
  Publisher(const ChannelIdType& channel);
  Publisher(const ChannelIdType& channel,
            std::unique_ptr<const EventBase> event);
  ~Publisher() = default;

  Publisher(const Publisher&) = delete;
  const Publisher& operator=(const Publisher&) = delete;

  void Publish(std::unique_ptr<const EventBase> event);

  inline const ChannelIdType& get_channel_id() { return channel_id_; }
  inline const ChannelIdType& get_response_channel_id() {
    return response_channel_id_;
  }

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
  Listener();
  Listener(const ChannelIdType& channel);
  ~Listener() = default;

  bool SubscribeTo(const ChannelIdType& channel);

  const std::shared_ptr<const EventBase> ReadLatest();

  inline bool Wait(int ms = 0) {
    return ValidatePublisher() ? publisher_->Wait(ms, read_index_) : false;
  }
  inline bool HasNews() {
    return ValidatePublisher() ? publisher_->HasNext(read_index_) : false;
  }
  void EnableCallback(std::function<void()> callback) {}

 private:
  inline bool Listener::ValidatePublisher() const {
    return publisher_ ? true
                      : (event_bus_->GetPublisher(channel_id_) != nullptr);
  }

 private:
  ChannelIdType channel_id_;
  std::shared_ptr<Publisher> publisher_;

  std::shared_ptr<EventBus> event_bus_;
  size_t read_index_ = 0;
};

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