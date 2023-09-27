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
#include "src/habitify_core/event_bus/event.h"

#include <gtest/gtest.h>

#include <memory>

namespace habitify_core {
namespace habitify_testing {
namespace {
TEST(EventTest, ConstructionAndConversion) {
  int ie = 12;
  ::habitify_core::Event<int> int_event{::habitify_core::EventType::TEST, 0,
                                        &ie};
  std::unique_ptr<::habitify_core::internal::EventBase> int_event_base =
      std::make_unique(int_event);
  EXPECT_EQ(int_event_base->MutableGetData<int>(), &ie);
  EXPECT_EQ(*int_event_base->GetData<int>(), ie);
  // TODO: Once assert for type missmatch is added to EventBase I need to check
  // for EXPECT_DEATH
}
}  // namespace
}  // namespace habitify_testing
}  // namespace habitify_core
