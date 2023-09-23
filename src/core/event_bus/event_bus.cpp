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

  channel_ = event_bus_->GetChannel(channel);

  if (channel_) {
    // The Publisher is stored as a shared_ptr by the channel to properly handle
    // its lifetime.
    channel_->RegisterPublisher(shared_from_this());
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
  if (std::find(listeners_.begin(), listeners_.end(), listener) !=
      listeners_.end())
    return;
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

  publisher_ = publisher;

  // Since a new Publisher was assigned to the channel we need to update all
  // Listeners that are already subscribed to this channel.
  for (auto& listener : listeners_) {
    listener->SubscribeTo(channel_id_);
  }

  return publisher_;
}

}  // namespace internal

Listener::Listener() : event_bus_(EventBus::get_instance()) {}

Listener::Listener(const ChannelIdType& channel)
    : event_bus_(EventBus::get_instance()), channel_id_(channel) {
  this->SubscribeTo(channel);
}

bool Listener::SubscribeTo(const ChannelIdType& channel) {
  std::unique_lock<std::shared_mutex> lock(mux_);
  channel_ = event_bus_->GetChannel(channel);
  if (channel_) {
    // The Listener is stored as a shared_ptr by the channel to properly handle
    // its lifetime.
    channel_->RegisterListener(shared_from_this());
    publisher_ = channel_->get_publisher();
    channel_id_ = channel;
    read_index_ = 0;
    return true;
  }
  return false;
}

bool Listener::ValidatePublisher() {
  if (!publisher_)
    // In case the Publisher was deleted we try to get a new one from the
    // channel.
    publisher_ = channel_ ? channel_->get_publisher() : nullptr;
  return (bool)publisher_;
}

std::shared_ptr<internal::Channel> EventBus::GetChannel(
    const ChannelIdType& channel) {
  std::shared_lock<std::shared_mutex> lock(mux_);
  auto it = channels_.find(channel);
  if (it != channels_.end()) return it->second;

  // If the channel does not exist yet we create it.
  auto channel_ptr = std::make_shared<internal::Channel>(channel);
  channels_.emplace(std::make_pair(channel, channel_ptr));

  return channel_ptr;
}
}  // namespace habitify_core
