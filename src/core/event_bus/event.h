#ifndef HABITIFY_SRC_CORE_EVENT_BUS_EVENT_H_
#define HABITIFY_SRC_CORE_EVENT_BUS_EVENT_H_

namespace habitify_core {

enum EventType { TEST };

using ChannelIdType = int;

class EventBase {
 public:
  EventBase() = default;
  EventBase(EventType etype, ChannelIdType channel_id = 0)
      : event_type_(etype), channel_id_(channel_id) {}
  virtual ~EventBase() {}

  inline const EventType &get_event_type() const { return event_type_; }
  inline const ChannelIdType &get_channel_id() const { return channel_id_; }

  inline void set_event_type(const EventType &etype) { event_type_ = etype; }
  inline void set_channel_id(const ChannelIdType &id) { channel_id_ = id; }

  template <typename T>
  T *MutableGetData() {
    return static_cast<T *>(GetMutableDataImpl());
  }

  template <typename T>
  const T &GetData() const {
    return static_cast<const T>(GetMutableDataImpl());
  }

 protected:
  virtual void *GetMutableDataImpl() { return nullptr; }

 private:
  EventType event_type_;
  ChannelIdType channel_id_ = 0;
};

template <typename T>
class Event : public EventBase {
 public:
  Event(EventType etype, ChannelIdType channel_id, T *data)
      : EventBase(etype, channel_id), data_(data) {}
  ~Event() {}

 protected:
  virtual void *GetDataImpl() override { return data_; }

 private:
  T *data_;
};

}  // namespace habitify_core

#endif  // HABITIFY_SRC_CORE_EVENT_BUS_EVENT_H_