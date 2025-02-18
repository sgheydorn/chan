#ifndef _TEST_UTIL_UNBOUNDED_TRY
#define _TEST_UTIL_UNBOUNDED_TRY

#include <iostream>

namespace test_util {
template <typename S, typename R> int unbounded_try(S tx, R rx) {
  {
    auto result = rx.try_recv();
    if (result) {
      std::cerr << "try_recv succeeded when it should have failed";
      return 1;
    }
    if (!result.error().is_empty()) {
      std::cerr << "expected try_recv error to be \"empty\" but it was not";
      return 1;
    }
  }
  {
    tx.send(3);
    auto result = rx.try_recv();
    if (!result) {
      std::cerr << "try_recv failed when it should not have";
      return 1;
    }
    if (*result != 3) {
      std::cerr << "wrong item from try_recv: expected 3 got " << *result;
      return 1;
    }
  }
  {
    auto result = rx.try_recv_for(std::chrono::microseconds(1));
    if (result) {
      std::cerr << "try_recv_for succeeded when it should have failed";
      return 1;
    }
    if (!result.error().is_empty()) {
      std::cerr << "expected try_recv_for error to be \"empty\" but it was not";
      return 1;
    }
  }
  {
    tx.send(4);
    auto result = rx.try_recv_for(std::chrono::microseconds(1));
    if (!result) {
      std::cerr << "try_recv_for failed when it should not have";
      return 1;
    }
    if (*result != 4) {
      std::cerr << "wrong item from try_recv_for: expected 4 got " << *result;
      return 1;
    }
  }
  {
    auto result = rx.try_recv_until(std::chrono::steady_clock::now() +
                                    std::chrono::microseconds(1));
    if (result) {
      std::cerr << "try_recv_until succeeded when it should have failed";
      return 1;
    }
    if (!result.error().is_empty()) {
      std::cerr
          << "expected try_recv_until error to be \"empty\" but it was not";
      return 1;
    }
  }
  {
    tx.send(5);
    auto result = rx.try_recv_until(std::chrono::steady_clock::now() +
                                    std::chrono::microseconds(1));
    if (!result) {
      std::cerr << "try_recv_until failed when it should not have";
      return 1;
    }
    if (*result != 5) {
      std::cerr << "wrong item from try_recv_until: expected 5 got " << *result;
      return 1;
    }
  }

  return 0;
}
} // namespace test_util

#endif
