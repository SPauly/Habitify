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
  // EventBus needs to be a friend class to properly register the Publisher to a
  // channel
  friend class ::habitify_core::EventBus;

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
  inline const std::shared_ptr<EventBus> get_event_bus() { return event_bus_; }
  /// Returns the latest Event as it's base class. This is mostly used for
  /// testing. Prefer accessing the Data via a Listener object.
  const std::shared_ptr<const internal::EventBase> GetLatestEvent() {
    return ReadLatestImpl();
  }

  /// PublisherBase::HasNews(size_t index) checks if there are unread events for
  /// the caller based on the specified index. TODO: take into account that
  /// events can be removed from the que.
  virtual bool HasNews(size_t index) {
    assert(false && "HasNews() not implemented");
    return true;
  }

 protected:
  /// This function is called by Listener::ReadLatest and is implemented by
  /// the derived class.
  virtual const std::shared_ptr<const internal::EventBase> ReadLatestImpl() {
    return nullptr;
  }

  /// RegisterPublisher(const ChannelIdType& channel) is called by EventBus and
  /// sets all the necessary members.
  bool RegisterPublisher(const std::shared_ptr<Channel> channel,
                         const std::shared_ptr<EventBus> event_bus);

 protected:
  mutable std::shared_mutex mux_;
  std::shared_ptr<std::condition_variable_any> cv_;
  std::shared_ptr<Channel> channel_;

 private:
  bool is_registered_ = false;
  /// channel_id_ refers to a predefined ChannelId and is used for
  /// identification by the Listener.
  ChannelIdType channel_id_ = 0;
  std::shared_ptr<Channel> channel_;
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
///       p->RegisterPublisher(0);
///       p.Publish(std::move(event));
template <typename EvTyp>
class Publisher : public internal::PublisherBase {
 public:
  /// Publisher() was made private to ensure that it is only created via the
  /// Create function. This way we can enforce that Publisher is purely used as
  /// shared_ptr instance.
  inline std::shared_ptr<Publisher<EvTyp>> Create() {
    return std::shared_ptr<Publisher<EvTyp>>(new Publisher<EvTyp>());
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

  inline const size_t get_writer_index() { return writer_index_; }

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
  virtual ~Listener() = default;

  // Listener is not copyable due to the use of std::shared_mutex
  Listener(const Listener&) = delete;
  const Listener& operator=(const Listener&) = delete;

  /// Listener() was made private to ensure that it is only created via the
  /// Create function. This way we can enforce that Listener is purely used as
  /// shared_ptr instance.
  inline std::shared_ptr<Listener> Create() {
    return std::shared_ptr<Listener>(new Listener());
  }

  /// Attempts to subscribe to the specified channel. If no Publisher is set it
  /// creates a new Channel. The Channel then calls RefreshPublisher() once the
  /// Publisher was added.
  void ChangeSubscription(const ChannelIdType& channel);

  /// Returns true if the Listener is subscribed to a Publisher. And false if no
  /// publisher is set.
  inline bool ValidatePublisher() { return (bool)publisher_; }

  /// RefreshPublisher() is called by the Channel if a Publisher is added to it.
  inline void RefreshPublisher() {
    std::unique_lock<std::shared_mutex> lock(mux_);
    publisher_ = channel_->get_publisher();
  }

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

  // Getters
  inline const bool get_is_subscribed() { return is_subscribed_; }
  inline const ChannelIdType get_channel_id() { return channel_id_; }
  inline const size_t get_read_index() { return read_index_; }
  inline const std::shared_ptr<EventBus> get_event_bus() { return event_bus_; }

 protected:
  /// Listener::SubscribeTo() is used
  /// by the EventBus to assign the Listener to a specific channel
  void SubscribeTo(std::shared_ptr<internal::Channel> channel,
                   std::shared_ptr<EventBus> event_bus);

 private:
  Listener();

 private:
  std::shared_mutex mux_;
  bool is_subscribed_ = false;
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
/// EventBus is designed to be thread safe and used via a shared_ptr. Usage:
///       std::shared_ptr<EventBus> eb = EventBus::Create();
class EventBus : public std::enable_shared_from_this<EventBus> {
 public:
  // EventBus() is private since this should only be created via Create().
  ~EventBus() = default;

  // EventBus is noncopyable
  EventBus(const EventBus&) = delete;
  EventBus& operator=(const EventBus&) = delete;

  /// Construct and access the EventBus via this function.
  /// NOTE that EventBus cannot be constructed via a constructor.
  /// It can be more efficient to store the returned shared_ptr for future use
  /// than to call this function.
  inline std::shared_ptr<EventBus> Create() {
    return std::make_shared<EventBus>();
  }

  /// Returns a shared_ptr to the Listener object that is subscribed to the
  /// specified channel. This is the only way to obtain a Listener object.
  std::shared_ptr<Listener> SubscribeTo(const ChannelIdType& channel);

  /// Returns a shared_ptr to the Publisher object that publishes to the
  /// specified channel
  template <typename EvTyp>
  std::shared_ptr<Publisher<EvTyp>> RegisterPublisher(
      const ChannelIdType& channel) {
    std::unique_lock<std::shared_mutex> lock(mux_);

    auto channel_ptr = GetChannel(channel);
    // If the channel already has a publisher we avoid creating a new one. And
    // instead share the access to it.
    if (channel_ptr->get_publisher() != nullptr)
      return channel_ptr->get_publisher();

    auto publisher = Publisher<EvTyp>::Create(channel);
    publisher->RegisterPublisher(channel_ptr, shared_from_this());
    channel_ptr->RegisterPublisher(publisher);

    return publisher;
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
  std::shared_ptr<std::condition_variable_any> cv_;

  // Channels are stored together with their ID for fast lookups.
  std::unordered_map<ChannelIdType, std::shared_ptr<internal::Channel>>
      channels_;
};

}  // namespace habitify_core

#endif  // HABITIFY_SRC_CORE_EVENT_BUS_EVENT_BUS_H_
