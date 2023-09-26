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
#include <gtest/gtest.h>

#include <memory>

#include "src/habitify_core/event_bus/event.h"
#include "src/habitify_core/event_bus/event_bus.h"

namespace habitify_core {
namespace habitify_testing {
namespace {
class ChannelTest : public ::testing::Test {
 protected:
  ChannelTest() {
    publisher_ = ::habitify_core::Publisher<in>::Create();
    listener_ = ::habitify_core::Listener::Create();
  }
  std::shared_ptr<::habitify_core::Publisher<int>> publisher_;
  std::shared_ptr<::habitify_core::Listener> listener_;
  std::shared_ptr<::habitify_core::internal::Channel> channel_;
};

TEST_F(ChannelTest, ChannelInitialization) {
  ASSERT_NE(channel_ = std::make_shared<::habitify_core::internal::Channel>(
                1, publisher_),
            nullptr);
  EXPECT_EQ(channel_->get_channel_id(), 1);
  EXPECT_EQ(channel_->get_publisher(), publisher_);
}

TEST_F(ChannelTest, SubscriptionFunctionality) {
  listener_->SubscribeTo(1);
  EXPECT_EQ(channel_->get_listeners().at(0), listener_);
}

}  // namespace
}  // namespace habitify_testing

}  // namespace habitify_core
