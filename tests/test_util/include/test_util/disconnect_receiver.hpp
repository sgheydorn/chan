#ifndef _TEST_UTIL_DISCONNECT_RECEIVER_HPP
#define _TEST_UTIL_DISCONNECT_RECEIVER_HPP

#include <iostream>

namespace test_util {
template <typename S, typename R> int disconnect_receiver(S tx, R rx) {
  rx.disconnect();

  {
    auto result = tx.send(7);

    if (result) {
      std::cerr << "expected send to fail but it succeeded";
      return 1;
    }

    if (result.error().item != 7) {
      std::cerr << "wrong item in send error: expected 7 got "
                << result.error().item;
      return 1;
    }
  }

  {
    auto result = tx.try_send(7);

    if (result) {
      std::cerr << "expected try_send to fail but it succeeded";
      return 1;
    }

    if (result.error().item != 7) {
      std::cerr << "wrong item in try_send error: expected 7 got "
                << result.error().item;
      return 1;
    }

    if (!result.error().is_disconnected()) {
      std::cerr << "expected error from try_send to be \"disconnected\" but it "
                   "was not";
      return 1;
    }
  }

  {
    auto result = tx.try_send_for(7, std::chrono::microseconds(1));

    if (result) {
      std::cerr << "expected try_send_for to fail but it succeeded";
      return 1;
    }

    if (result.error().item != 7) {
      std::cerr << "wrong item in try_send_for error: expected 7 got "
                << result.error().item;
      return 1;
    }

    if (!result.error().is_disconnected()) {
      std::cerr << "expected error from try_send_for to be \"disconnected\" "
                   "but it was not";
      return 1;
    }
  }

  {
    auto result = tx.try_send_until(7, std::chrono::steady_clock::now() +
                                           std::chrono::microseconds(1));

    if (result) {
      std::cerr << "expected try_send_until to fail but it succeeded";
      return 1;
    }

    if (result.error().item != 7) {
      std::cerr << "wrong item in try_send_until error: expected 7 got "
                << result.error().item;
      return 1;
    }

    if (!result.error().is_disconnected()) {
      std::cerr << "expected error from try_send_until to be \"disconnected\" "
                   "but it was not";
      return 1;
    }
  }

  return 0;
}
} // namespace test_util

#endif
