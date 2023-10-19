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
#include <string>

#include "src/core/event_bus/event.h"
#include "src/core/event_bus/event_bus.h"

namespace habitify_core {
namespace habitify_testing {
namespace {

class EventBusTest : public ::testing::Test {
 protected:
  void SetUp() override {
    event_bus_ = ::habitify_core::EventBus::Create();
    listener_int_ = event_bus_->SubscribeTo(0);
    listener_str_ = event_bus_->SubscribeTo(1);

    publisher_int_ = event_bus_->RegisterPublisher<int>(0);
    publisher_str_ = event_bus_->RegisterPublisher<std::string>(1);
  }

 protected:
  // utils
  std::shared_ptr<EventBus> event_bus_;
  int test_value_ = 418;
  ::habitify_core::Event<int> event_int_{::habitify_core::EventType::TEST, 0,
                                         &test_value_};
  std::string test_string_ = "test";
  ::habitify_core::Event<std::string> event_str_{
      ::habitify_core::EventType::TEST, 1, &test_string_};

  // Listeners
  std::shared_ptr<::habitify_core::Listener> listener_int_;
  std::shared_ptr<::habitify_core::Listener> listener_str_;

  // Publishers
  std::shared_ptr<::habitify_core::Publisher<int>> publisher_int_;
  std::shared_ptr<::habitify_core::Publisher<std::string>> publisher_str_;
};

TEST_F(EventBusTest, Initialization) {
  // Check if all pointers are initialized properly
  EXPECT_TRUE(event_bus_ != nullptr);
  EXPECT_TRUE(listener_int_ != nullptr);
  EXPECT_TRUE(listener_str_ != nullptr);
  EXPECT_TRUE(publisher_int_ != nullptr);
  EXPECT_TRUE(publisher_str_ != nullptr);

  EXPECT_EQ(event_bus_->GetChannelCount(), 2);
}

TEST_F(EventBusTest, PublishAndReceive) {
  // Publish an event and check if it is received
  ASSERT_TRUE(
      publisher_int_->Publish(std::make_unique<const Event<int>>(event_int_)));
  EXPECT_TRUE(listener_int_->HasReceivedEvent());
  auto latest_event_int_ = listener_int_->ReadLatest<int>();
  EXPECT_EQ(*latest_event_int_->GetData<int>(), test_value_);

  // Check if the counters are increased
  EXPECT_EQ(publisher_int_->get_writer_index(), 1);
  EXPECT_EQ(listener_int_->get_read_index(), 1);

  // Test functionality with a string
  ASSERT_TRUE(publisher_str_->Publish(
      std::make_unique<const Event<std::string>>(event_str_)));
  EXPECT_TRUE(listener_str_->HasReceivedEvent());
  auto latest_event_str_ = listener_str_->ReadLatest<std::string>();
  EXPECT_EQ(*latest_event_str_->GetData<std::string>(), test_string_);
}

}  // namespace

}  // namespace habitify_testing

}  // namespace habitify_core

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
