// Habitify - Habit tracking and creating platform
// Copyright (C) 2023  Simon Pauly
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Contact via <https://github.com/SPauly/Habitify>

/// This file implements a thread safe EventBus system for communication between
/// the different compartments. Data is shared in a Publisher-Listener pattern
/// were each Publisher can share events of a fixed type. The Listeners can read
/// or pull these events from the Publisher.
/// Interface:
///       - template<typename EvType>Publisher: this interface to the EventBus
///       stores the events and takes care of data management and sharing.
///       - Listener serves as interface to the Publisher and exposes reading
///       functionality.
///           NOTE: EvType is the type which is used to instatiate the Event<T>
///           object e.g. Event<int>
///           NOTE: Listener and Publisher need to be created as shared_ptr to
///           ensure thread safety.
///                 e.g. std::shared_ptr<Listener> l = Listener::Create();
///       - EventBus: the EventBus is a singleton that stores the matching
///       Publisher and Listener objects together. It should only be accessed
///       via Publisher and Listener

#ifndef HABITIFY_SRC_CORE_EVENT_BUS_EVENT_BUS_H_
#define HABITIFY_SRC_CORE_EVENT_BUS_EVENT_BUS_H_

#include <cassert>
#include <condition_variable>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "src/core/event_bus/event.h"

namespace habitify_core {

// Forward declarations
namespace internal {
class Channel;
}
class Listener;
class EventBus;

namespace internal {
/// PublisherBase is used as a way of storing Publisher in the EventBus. The
/// actual functionality is implemented by the derived class. It needs to be
/// inherated from. Use Publisher as the interface to EventBus!
class PublisherBase : public std::enable_shared_from_this<PublisherBase> {
 public:
  // The Listener needs to be a friend class to access the provided ReadFoo()
  // functions.
  friend class ::habitify_core::Listener;

  PublisherBase();
  virtual ~PublisherBase() = default;

  // PublisherBase is not copyable due to the use of std::shared_mutex
  PublisherBase(const PublisherBase&) = delete;
  const PublisherBase& operator=(const PublisherBase&) = delete;

  // Getters and Setters:
  inline const ChannelIdType& get_channel_id() { return channel_id_; }
  inline const bool get_is_registered() { return is_registered_; }
  /// Returns a conditonal_variable_any that is notified by Publish().
  inline std::shared_ptr<std::condition_variable_any> get_cv() { return cv_; }
  /// Returns the latest Event as it's base class. This is mostly used for
  /// testing. Prefer accessing the Data via a Listener object.
  const std::shared_ptr<const internal::EventBase> GetLatestEvent() {
    return ReadLatestImpl();
  }

  /// PublisherBase::HasNews(size_t index) checks if there are unread events for
  /// the caller based on the specified index. TODO: take into account that
  /// events can be removed from the que.
  virtual bool HasNews(size_t index) { return false; }

  /// RegisterChannel(const ChannelIdType& channel) tries to add the
  /// Publisher to the EventBus on the specified channel.
  /// Returns false if the attempt fails or if the Publisher already has been
  /// registered. NOTE that this should only be called once.
  bool RegisterChannel(const ChannelIdType& channel);

 protected:
  /// This function is called by Listener::ReadLatest and is implemented by
  /// the derived class.
  virtual const std::shared_ptr<const internal::EventBase> ReadLatestImpl() {
    return nullptr;
  }

 protected:
  mutable std::shared_mutex mux_;
  std::shared_ptr<std::condition_variable_any> cv_;
  std::shared_ptr<Channel> channel_;

 private:
  bool is_registered_ = false;
  /// channel_id_ refers to a predefined ChannelId and is used for
  /// identification by the Listener.
  ChannelIdType channel_id_ = 0;
  std::shared_ptr<EventBus> event_bus_;
};

/// Channel is used to store the Publisher and Listener objects together.
/// It is used internally by the EventBus and should not be used directly.
class Channel {
 public:
  Channel() = delete;
  Channel(const ChannelIdType& channel_id,
          std::shared_ptr<PublisherBase> publisher = nullptr);
  ~Channel() = default;

  // Channel is not copyable due to the use of std::shared_mutex
  Channel(const Channel&) = delete;
  const Channel& operator=(const Channel&) = delete;

  // Accessors
  inline const ChannelIdType& get_channel_id() { return channel_id_; }
  inline const std::shared_ptr<PublisherBase> get_publisher() {
    return publisher_;
  }
  inline const std::vector<std::shared_ptr<Listener>> get_listeners() {
    return listeners_;
  }
  /// Adds a new Listener to the Channel.
  void RegisterListener(std::shared_ptr<Listener> listener);

  /// Registers the Publisher. TODO: We need to return a nullptr or break if the
  /// EvTyps of publisher do not match. When they do we can merge them.
  std::shared_ptr<PublisherBase> RegisterPublisher(
      std::shared_ptr<PublisherBase> publisher);

 private:
  std::shared_mutex mux_;

  ChannelIdType channel_id_;
  std::shared_ptr<PublisherBase> publisher_;
  std::vector<std::shared_ptr<Listener>> listeners_;
};
}  // namespace internal

/// Publisher manages objects of type Event<EvTyp> and publishes the data to the
/// Listener.
/// It is designed to be thread safe so that multiple Listeners can access the
/// data concurrently.
/// Usage:
///       std::unique_ptr<Event<int>> event;
///       std::shared_ptr<Publisher<int>> p = Publisher<int>::Create();
///       p->RegisterChannel(0);
///       p.Publish(std::move(event));
template <typename EvTyp>
class Publisher : public internal::PublisherBase {
 public:
  /// Publisher() was made private to ensure that it is only created via the
  /// Create function. This way we can enforce that Publisher is purely used as
  /// shared_ptr instance.
  static std::shared_ptr<Publisher<EvTyp>> Create() {
    return std::shared_ptr<Publisher<EvTyp>>(new Publisher<EvTyp>());
  }
  static std::shared_ptr<Publisher<EvTyp>> Create(
      const ChannelIdType& channel) {
    auto ret = std::shared_ptr<Publisher<EvTyp>>(new Publisher<EvTyp>());
    // RegisterChannel cannot be called from inside the constructor of Publisher
    // since it needs to create a shared_from_this() which is not possible
    // before the object is fully constructed.
    ret->RegisterChannel(channel);
    return ret;
  }
  ~Publisher() = default;

  // Publisher is not copyable due to the use of std::shared_mutex
  Publisher(const Publisher&) = delete;
  const Publisher& operator=(const Publisher&) = delete;

  /// Publisher::HasNews(size_t index) checks if there are unread events for the
  /// Listener
  virtual bool HasNews(size_t index) override {
    std::shared_lock<std::shared_mutex> lock(mux_);
    return index < writer_index_;
  }

  /// Publisher<EvTyp>::Publish(std::unique_ptr< const internal::EventBase>)
  /// takes ownership of the event and provides thread safe access to the
  /// Listener.
  template <typename T>
  bool Publish(std::unique_ptr<const Event<T>> event) {
    if (!get_is_registered()) return false;
    std::unique_lock<std::shared_mutex> lock(mux_);

    auto shared_event =
        std::shared_ptr<const internal::EventBase>(std::move(event));
    event_storage_.emplace(writer_index_, shared_event);

    cv_->notify_all();
    ++writer_index_;
    return true;
  }

 protected:
  /// See PublisherBase::ReadLatestImpl()
  virtual const std::shared_ptr<const internal::EventBase> ReadLatestImpl()
      override {
    std::shared_lock<std::shared_mutex> lock(mux_);

    if (event_storage_.empty()) return nullptr;

    auto event = event_storage_.find(writer_index_ - 1);
    if (event == event_storage_.end()) return nullptr;

    return event->second;
  }

 private:
  Publisher() : PublisherBase() {}

 private:
  std::unordered_map<int, std::shared_ptr<const internal::EventBase>>
      event_storage_;
  size_t writer_index_ = 0;
};

/// Listener is used to read events from the Publisher. It is designed to be
/// thread safe. Usage:
///       std::shared_ptr<Listener> l = Listener::Create();
///       l->SubscribeTo(0);
///       if(l.HasNews()) auto event = l.ReadLatest<int>();
class Listener : public std::enable_shared_from_this<Listener> {
 public:
  /// Listener() was made private to ensure that it is only created via the
  /// Create function. This way we can enforce that Listener is purely used as
  /// shared_ptr instance.
  static std::shared_ptr<Listener> Create() {
    return std::shared_ptr<Listener>(new Listener());
  }
  static std::shared_ptr<Listener> Create(const ChannelIdType& channel) {
    auto new_channel = std::shared_ptr<Listener>(new Listener());
    new_channel->SubscribeTo(channel);
    return new_channel;
  }
  virtual ~Listener() = default;

  // Listener is not copyable due to the use of std::shared_mutex
  Listener(const Listener&) = delete;
  const Listener& operator=(const Listener&) = delete;

  /// Attempts to subscribe to the specified channel. If no Publisher is set it
  /// creates a new Channel. The Channel then calls SubscribeTo() again to set
  /// the Publisher once one was added.
  bool SubscribeTo(const ChannelIdType& channel);

  /// Checks if the Publisher is valid and if the Listener is subscribed to it.
  /// Attempts to subscribe to the channel if not.
  bool ValidatePublisher();

  /// Returns the latest event published by the Publisher. If there are no
  /// events it returns nullptr.
  template <typename EvTyp>
  const std::shared_ptr<const Event<EvTyp>> ReadLatest() {
    std::shared_lock<std::shared_mutex> lock(mux_);

    if (!ValidatePublisher()) return nullptr;

    auto event = publisher_->ReadLatestImpl();
    if (event == nullptr) return nullptr;

    auto latest_converted = std::static_pointer_cast<const Event<EvTyp>>(event);
    if (!latest_converted)
      assert(false && "ReadLatest tried retrieving data of wrong format");

    return latest_converted;
  }

  inline bool HasNews() {
    return ValidatePublisher() ? publisher_->HasNews(read_index_) : false;
  }

  inline const ChannelIdType get_channel_id() { return channel_id_; }

 private:
  Listener();
  Listener(const ChannelIdType& channel);

 private:
  std::shared_mutex mux_;
  size_t read_index_ = 0;

  /// channel_id_ refers to a predefined ChannelId and is used to identify the
  /// Publisher.
  ChannelIdType channel_id_ = 0;
  std::shared_ptr<internal::Channel> channel_;

  /// This might be nullptr if the Listener is not subscribed to a Publisher.
  std::shared_ptr<internal::PublisherBase> publisher_;
  std::shared_ptr<EventBus> event_bus_;
};

/// EventBus establishes the connection between Publisher and Listener objects.
/// Each Publisher is assigned to one or more Channel object. Each Channel
/// however is limited to one MessageType. Multiple Listener objects can
/// subscribe to that Channel.
/// EventBus is designed to be thread safe and used as a singleton. The class is
/// mostly used internally.
/// Use Publisher and Listener as interface.
class EventBus {
 public:
  // PublisherBase and Listener need to be friend classes to get access to the
  // channel.
  friend class internal::PublisherBase;
  friend class Listener;

  // EventBus() is private since this is a singleton
  ~EventBus() = default;

  // EventBus is noncopyable
  EventBus(const EventBus&) = delete;
  EventBus& operator=(const EventBus&) = delete;

  /// Construct and access the EventBus via this function.
  /// NOTE that EventBus cannot be constructed via a constructor.
  /// It can be more efficient to store the returned shared_ptr for future use
  /// than to call this function.
  inline static std::shared_ptr<EventBus> get_instance() {
    static std::shared_ptr<EventBus> instance(new EventBus);
    return instance;
  }

 protected:
  /// Returns the Channel with the specified ID. If no Channel with that ID
  /// exists it instantiates a new one.
  std::shared_ptr<internal::Channel> GetChannel(
      const ChannelIdType& channel_id);

 private:
  // This is a singleton class so the constructor needs to be private.
  EventBus() = default;

 private:
  mutable std::shared_mutex mux_;

  // Channels are stored together with their ID for fast lookups.
  std::unordered_map<ChannelIdType, std::shared_ptr<internal::Channel>>
      channels_;
};

}  // namespace habitify_core

#endif  // HABITIFY_SRC_CORE_EVENT_BUS_EVENT_BUS_H_
