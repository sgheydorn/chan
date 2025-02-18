#ifndef _TEST_UTIL_ONE_ITEM
#define _TEST_UTIL_ONE_ITEM

#include <iostream>

namespace test_util {
template <typename S, typename R> int one_item(S tx, R rx) {
  if (tx.channel_capacity() != 16) {
    std::cerr << "expected channel capacity (from sender) to be 16 but it is "
              << tx.channel_capacity();
    return 1;
  }

  if (rx.channel_capacity() != 16) {
    std::cerr << "expected channel capacity (from receiver) to be 16 but it is "
              << rx.channel_capacity();
    return 1;
  }

  if (!tx.send(7)) {
    std::cerr << "sender disconnected when it should not be";
    return 1;
  }

  if (tx.channel_size() != 1) {
    std::cerr << "expected channel size (from sender) to be 1 but it is "
              << tx.channel_size();
    return 1;
  }

  auto item = rx.recv();
  if (!item) {
    std::cerr << "receiver disconnected when it should not be";
    return 1;
  }

  if (rx.channel_size() != 0) {
    std::cerr << "expected channel size (from receiver) to be 0 but it is "
              << rx.channel_size();
    return 1;
  }

  if (*item != 7) {
    std::cerr << "wrong item from recv: expected 7 got " << *item;
    return 1;
  }

  return 0;
}
} // namespace test_util

#endif
