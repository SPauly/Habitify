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
TEST_F(PublisherBaseTest, ConstructPublisherBase) {
  std::shared_ptr<::habitify_core::internal::PublisherBase> publisher_base =
      std::make_shared<::habitify_core::internal::PublisherBase>();
  EXPECT_EQ(publisher_base->get_channel_id(), 0);
  EXPECT_EQ(publisher_base->get_is_registered(), false);
  ASSERT_NE(publisher_base->get_cv(), nullptr);
}

// Tests that the PublisherBase properly registers a channel.
TEST_F(PublisherBaseTest, RegisterPublisher) {
  ASSERT_EQ(publisher_base_->get_is_registered(), false);
  publisher_base_->RegisterChannel(1);
  ASSERT_EQ(publisher_base_->get_channel_id(), 1);
  ASSERT_EQ(publisher_base_->get_is_registered(), true);
}

}  // namespace
}  // namespace habitify_testing
}  // namespace habitify_core

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}