#ifndef HABITIFY_SRC_CORE_EVENT_BUS_EVENT_BUS_H_
#define HABITIFY_SRC_CORE_EVENT_BUS_EVENT_BUS_H_

#include <deque>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "src/core/event_bus/event.h"
#include "src/core/event_bus/listener_publisher.h"
namespace habitify_core {

class EventBus {
 public:
  EventBus() = delete;
  ~EventBus();

  inline static std::shared_ptr<EventBus> get_instance() {
    static std::shared_ptr<EventBus> instance(new EventBus);
    return instance;
  }

  // we use mutexes so this class needs to be noncopyable
  EventBus(const EventBus&) = delete;
  EventBus& operator=(const EventBus&) = delete;

 private:
  std::mutex mux_channels_;

  std::unordered_map<uint64_t, std::shared_ptr<Publisher>> channels_;
  std::deque<std::shared_ptr<Publisher>> cache_;
};

}  // namespace habitify_core

#endif  // HABITIFY_SRC_CORE_EVENT_BUS_EVENT_BUS_H_

/*
Usage:

Event<proto::PingMessage> e("hello");
Publisher p;
auto ret_channel = p.publish(event_bus, e, true);
Listener l(ret_channel);
if(l.wait())
    std::string response = l.pull<std::string>();
*/