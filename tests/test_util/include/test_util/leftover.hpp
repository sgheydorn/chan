#ifndef _TEST_UTIL_LEFTOVER
#define _TEST_UTIL_LEFTOVER

#include <iostream>
#include <memory>

namespace test_util {
template <typename S, typename R> int leftover(S tx, R rx) {
  for (int i = 0; i < 7; ++i) {
    if (!tx.send(std::make_unique<int>(i))) {
      std::cerr << "send failed when it should not have";
      return 1;
    }
  }
  return 0;
}
} // namespace test_util

#endif
