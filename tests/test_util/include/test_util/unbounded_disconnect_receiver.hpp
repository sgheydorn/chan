#ifndef _TEST_UTIL_UNBOUNDED_DISCONNECT_RECEIVER
#define _TEST_UTIL_UNBOUNDED_DISCONNECT_RECEIVER

#include <iostream>

namespace test_util {
template <typename S, typename R>
int unbounded_disconnect_receiver(S tx, R rx) {
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

  return 0;
}
} // namespace test_util

#endif
