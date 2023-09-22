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

#include "src/core/event_bus/event_bus.h"

#include <chrono>

namespace habitify_core {
namespace internal {
PublisherBase::PublisherBase()
    : cv_(std::make_shared<std::condition_variable_any>()),
      event_bus_(EventBus::get_instance()) {}
PublisherBase::PublisherBase(const ChannelIdType& channel) : PublisherBase() {
  TryRegisterChannel(channel);
}

bool PublisherBase::TryRegisterChannel(const ChannelIdType& channel) {
  std::unique_lock<std::shared_mutex> lock(mux_);
  if (is_registered_) return false;

  // The Publisher is stored as a shared_ptr by the channel to properly handle
  // its lifetime.
  channel_ = event_bus_->RegisterPublisher(channel, shared_from_this());

  if (channel_) {
    channel_id_ = channel;
    is_registered_ = true;
  }

  return is_registered_;
}

Channel::Channel(const ChannelIdType& channel,
                 std::shared_ptr<PublisherBase> publisher)
    : channel_id_(channel), publisher_(publisher) {}

void Channel::RegisterListener(std::shared_ptr<Listener> listener) {
  std::unique_lock<std::shared_mutex> lock(mux_);
  listeners_.push_back(listener);
}

std::shared_ptr<PublisherBase> Channel::RegisterPublisher(
    std::shared_ptr<PublisherBase> publisher) {
  std::unique_lock<std::shared_mutex> lock(mux_);
  // If the channel already has a publisher we merge them by assigning the given
  // shared_ptr to the publisher_ in place.
  if (publisher_) {
    publisher = publisher_;
    return publisher;
  }

  return publisher_ = publisher;
}

}  // namespace internal

template <typename EvTyp>
Publisher<EvTyp>::Publisher() : PublisherBase() {}

template <typename EvTyp>
Publisher<EvTyp>::Publisher(const ChannelIdType& channel)
    : PublisherBase(channel) {}

template <typename EvTyp>
Publisher<EvTyp>::Publisher(const ChannelIdType& channel,
                            std::unique_ptr<const EventBase> event)
    : Publisher(channel) {
  this->Publish(std::move(event));
}

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
