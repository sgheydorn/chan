#ifndef _TEST_UTIL_UNBUFFERED_DISCONNECT_SENDER_HPP
#define _TEST_UTIL_UNBUFFERED_DISCONNECT_SENDER_HPP

#include <iostream>

namespace test_util {
template <typename S, typename R> int unbuffered_disconnect_sender(S tx, R rx) {
  tx.disconnect();

  if (auto item = rx.recv(); item) {
    std::cerr << "expected recv to fail but it succeeded with item " << *item;
    return 1;
  }

  return 0;
}
} // namespace test_util

#endif
