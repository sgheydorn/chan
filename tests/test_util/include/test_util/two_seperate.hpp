#ifndef _TEST_UTIL_TWO_SEPERATE
#define _TEST_UTIL_TWO_SEPERATE

#include <iostream>

namespace test_util {
template <typename S, typename R> int two_seperate(S tx, R rx) {
  if (!tx.send(7)) {
    std::cerr << "sender disconnected when it should not be";
    return 1;
  }

  auto item1 = rx.recv();
  if (!item1) {
    std::cerr << "receiver disconnected when it should not be";
    return 1;
  }

  if (*item1 != 7) {
    std::cerr << "wrong item from recv: expected 7 got " << *item1;
    return 1;
  }

  if (!tx.send(13)) {
    std::cerr << "sender disconnected when it should not be";
    return 1;
  }

  auto item2 = rx.recv();
  if (!item2) {
    std::cerr << "receiver disconnected when it should not be";
    return 1;
  }

  if (*item2 != 13) {
    std::cerr << "wrong item from recv: expected 13 got " << *item2;
    return 1;
  }

  return 0;
}
} // namespace test_util

#endif
