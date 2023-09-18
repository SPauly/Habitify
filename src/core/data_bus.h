#ifndef HABITIFY_SRC_CORE_DATA_BUS_H_
#define HABITIFY_SRC_CORE_DATA_BUS_H_

#include <deque>
#include <mutex>
#include <thread>

namespace habitify_core {
class DataBus {
 public:
  DataBus();
  ~DataBus();

 private:
};

}  // namespace habitify_core

#endif  // HABITIFY_SRC_CORE_DATA_BUS_H_