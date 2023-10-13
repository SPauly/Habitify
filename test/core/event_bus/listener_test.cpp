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

#include "src/core/event_bus/event.h"
#include "src/core/event_bus/event_bus.h"

namespace habitify_core {
namespace habitify_testing {
namespace {
class ListenerTest : public ::testing::Test {
 protected:
  ListenerTest() {
    listener_ = ::habitify_core::Listener::Create();
    publisher_int_ = habitify_core::Publisher<int>::Create(1);
    publisher_string_ = habitify_core::Publisher<std::string>::Create(2);
  }
  std::shared_ptr<::habitify_core::Listener> listener_;

  std::shared_ptr<::habitify_core::Publisher<int>> publisher_int_;
  std::shared_ptr<::habitify_core::Publisher<std::string>> publisher_string_;
  int text_ev_int_ = 418;
  ::habitify_core::Event<int> event_int_{::habitify_core::EventType::TEST, 0,
                                         &text_ev_int_};
  std::string test_ev_string_ = "TestString";
  ::habitify_core::Event<std::string> event_string_{
      ::habitify_core::EventType::TEST, 0, &test_ev_string_};
};

TEST_F(ListenerTest, ConstructionAndSubscriptionListener) {
  EXPECT_NE(listener_, nullptr);
  EXPECT_EQ(listener_->get_channel_id(), 0);

  ASSERT_EQ(listener_->SubscribeTo(1), true) << "SubscribeTo fucntion fails";
  EXPECT_EQ(listener_->get_channel_id(), 1);

  listener_ = ::habitify_core::Listener::Create(2);
  EXPECT_NE(listener_, nullptr);
  EXPECT_EQ(listener_->get_channel_id(), 2);
}

TEST_F(ListenerTest, TestDataRetrieval) {
  // subscribe to Publisher<int>
  listener_->SubscribeTo(1);
  ASSERT_EQ(listener_->ValidatePublisher(), true);

  publisher_int_->Publish(std::make_unique<const Event<int>>(event_int_));

  auto response_int = listener_->ReadLatest<int>();
  ASSERT_NE(response_int, nullptr);
  EXPECT_EQ(*response_int->GetData<int>(), 418)
      << "Data retrieval for int not working.";

  // subscribe to Publisher<std::string>
  listener_->SubscribeTo(2);
  ASSERT_EQ(listener_->ValidatePublisher(), true);
  publisher_string_->Publish(
      std::make_unique<const Event<std::string>>(event_string_));
  EXPECT_STRCASEEQ(
      listener_->ReadLatest<std::string>()->GetData<std::string>()->c_str(),
      "TestString")
      << "Data retrieval for std::string not working.";

  // Test assertion of accessing the wrong data
  EXPECT_DEATH(listener_->ReadLatest<int>(),
               "ReadLatest tried retrieving data of wrong format")
      << "The assertion in ReadLatest does not work";
}

}  // namespace
}  // namespace habitify_testing
}  // namespace habitify_core