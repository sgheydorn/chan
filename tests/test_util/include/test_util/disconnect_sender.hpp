#ifndef _TEST_UTIL_DISCONNECT_SENDER_HPP
#define _TEST_UTIL_DISCONNECT_SENDER_HPP

#include <iostream>

namespace test_util {
template <typename S, typename R> int disconnect_sender(S tx, R rx) {
  tx.disconnect();

  if (auto item = rx.recv(); item) {
    std::cerr << "expected recv to fail but it succeeded with item " << *item;
    return 1;
  }

  {
    auto item = rx.try_recv();
    if (item) {
      std::cerr << "expected try_recv to fail but it succeeded with item "
                << *item;
      return 1;
    }

    if (!item.error().is_disconnected()) {
      std::cerr << "expected error from try_recv to be \"disconnected\" but it "
                   "was not";
      return 1;
    }
  }

  {
    auto item = rx.try_recv_for(std::chrono::microseconds(1));
    if (item) {
      std::cerr << "expected try_recv_for to fail but it succeeded with item "
                << *item;
      return 1;
    }

    if (!item.error().is_disconnected()) {
      std::cerr << "expected error from try_recv_for to be \"disconnected\" "
                   "but it was not";
      return 1;
    }
  }

  {
    auto item = rx.try_recv_until(std::chrono::steady_clock::now() +
                                  std::chrono::microseconds(1));
    if (item) {
      std::cerr << "expected try_recv_until to fail but it succeeded with item "
                << *item;
      return 1;
    }

    if (!item.error().is_disconnected()) {
      std::cerr << "expected error from try_until to be \"disconnected\" but "
                   "it was not";
      return 1;
    }
  }

  return 0;
}
} // namespace test_util

#endif
