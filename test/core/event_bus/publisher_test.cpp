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

// The fixture for testing class PublisherBase
class PublisherBaseTest : public ::testing::Test {
 protected:
  PublisherBaseTest() {
    publisher_base_ =
        std::make_shared<::habitify_core::internal::PublisherBase>();
  }
  std::shared_ptr<::habitify_core::internal::PublisherBase> publisher_base_;
};

// Tests that the PublisherBase constructor properly initializes the object.
TEST_F(PublisherBaseTest, IsInitializedProperly) {
  ASSERT_NE(publisher_base_, nullptr);
  EXPECT_EQ(publisher_base_->get_channel_id(), 0);
  EXPECT_EQ(publisher_base_->get_is_registered(), false);
  EXPECT_NE(publisher_base_->get_cv(), nullptr);
}

// Tests that the PublisherBase properly registers a channel.
TEST_F(PublisherBaseTest, IsRegisteredProperly) {
  ASSERT_NE(publisher_base_, nullptr);
  ASSERT_EQ(publisher_base_->RegisterPublisher(1), true);
  EXPECT_EQ(publisher_base_->get_channel_id(), 1);
  EXPECT_EQ(publisher_base_->get_is_registered(), true);
}

class PublisherTest : public ::testing::Test {
 protected:
  PublisherTest() {
    publisher_int_ = ::habitify_core::Publisher<int>::Create();
    publisher_string_ = ::habitify_core::Publisher<std::string>::Create();
  }
  std::shared_ptr<::habitify_core::Publisher<int>> publisher_int_;
  ::habitify_core::Event<int> event_int_{::habitify_core::EventType::TEST2, 0,
                                         0};
  std::shared_ptr<::habitify_core::Publisher<std::string>> publisher_string_;
  ::habitify_core::Event<std::string> event_string_{
      ::habitify_core::EventType::TEST, 0, nullptr};
};

// Tests that the Publisher constructor properly initializes the object.
TEST_F(PublisherTest, CreationWorks) {
  ASSERT_NE(publisher_int_, nullptr);
  EXPECT_EQ(publisher_int_->get_channel_id(), 0);
  EXPECT_EQ(publisher_int_->get_is_registered(), false);
  EXPECT_NE(publisher_int_->get_cv(), nullptr);

  std::shared_ptr<::habitify_core::Publisher<int>> publisher_test_c2 =
      ::habitify_core::Publisher<int>::Create(1);
  ASSERT_NE(publisher_test_c2, nullptr);
  ASSERT_EQ(publisher_test_c2->get_is_registered(), true);
  EXPECT_EQ(publisher_test_c2->get_channel_id(), 1);
}

TEST_F(PublisherTest, PublishCorrectness) {
  ASSERT_NE(publisher_int_, nullptr);
  ASSERT_EQ(publisher_int_->RegisterPublisher(1), true);
  EXPECT_EQ(
      publisher_int_->Publish(std::make_unique<const Event<int>>(event_int_)),
      true);
  // check that the Event properly was stored
  EXPECT_EQ(publisher_int_->HasReceivedEvent(0), true);
  EXPECT_EQ(publisher_int_->GetLatestEvent()->get_event_type(),
            ::habitify_core::EventType::TEST2);

  ASSERT_NE(publisher_string_, nullptr);
  ASSERT_EQ(publisher_string_->RegisterPublisher(2), true);
  EXPECT_EQ(publisher_string_->Publish(
                std::make_unique<const Event<std::string>>(event_string_)),
            true);
  EXPECT_EQ(publisher_string_->HasReceivedEvent(0), true);
  EXPECT_NE(publisher_int_->GetLatestEvent(), nullptr);

  // Assert that pushing a wrong Event<EvTyp> results in an assertion
  ASSERT_DEATH(publisher_int_->Publish(
                   std::make_unique<const Event<std::string>>(event_string_)),
               "Event must be of same type as EvTyp!");
}

TEST_F(PublisherTest, DetectNews) {
  ASSERT_NE(publisher_int_, nullptr);
  publisher_int_->RegisterPublisher(1);
  publisher_int_->Publish(std::make_unique<const Event<int>>(event_int_));
  EXPECT_EQ(publisher_int_->HasReceivedEvent(0), true);
}

}  // namespace
}  // namespace habitify_testing
}  // namespace habitify_core
