#ifndef _TEST_UTIL_BOUNDED_TRY
#define _TEST_UTIL_BOUNDED_TRY

#include <iostream>

namespace test_util {
template <typename S, typename R>
int bounded_try(S tx, R rx, std::size_t buffer_capacity) {
  for (int i = 0; i < int(buffer_capacity); ++i) {
    // Since try_send can spuriously return "full", try many times to make the
    // test consistent.
    for (int j = 0;; ++j) {
      if (j == 1000) {
        std::cerr << "try_send failed after 1000 attempts";
        return 0;
      }
      auto result = tx.try_send(i);
      if (result) {
        break;
      }
      if (result.error().is_disconnected()) {
        std::cerr << "try_send error was \"disconnected\" when it should have "
                     "succeeded";
        return 1;
      }
    }
  }
  {
    auto result = tx.try_send(3);
    if (result) {
      std::cerr << "expected try_send to fail but it succeeded";
      return 1;
    }
    if (!result.error().is_full()) {
      std::cerr << "expected try_send error to be \"full\" but it was not";
      return 1;
    }
    if (result.error().item != 3) {
      std::cerr << "wrong item in try_send error. expected 3 got "
                << result.error().item;
      return 1;
    }
  }
  for (int i = 0; i < int(buffer_capacity); ++i) {
    // Since try_recv can spuriously return "empty", try many times to make the
    // test consistent.
    int item;
    for (int j = 0;; ++j) {
      if (j == 1000) {
        std::cerr << "try_recv failed after 1000 attempts";
        return 1;
      }
      auto result = rx.try_recv();
      if (result) {
        item = *result;
        break;
      }
      if (result.error().is_disconnected()) {
        std::cerr << "try_recv error was \"disconnected\" when it should have "
                     "succeeded";
        return 1;
      }
    }
    if (item != i) {
      std::cerr << "wrong item from try_recv: expected " << i << " got "
                << item;
      return 1;
    }
  }
  {
    auto result = rx.try_recv();
    if (result) {
      std::cerr << "expected try_recv to fail but it succeeded";
      return 1;
    }
    if (!result.error().is_empty()) {
      std::cerr << "expected try_recv error to be \"empty\" but it was not";
      return 1;
    }
  }

  for (int i = 0; i < int(buffer_capacity); ++i) {
    if (auto result = tx.try_send_for(i, std::chrono::microseconds(1));
        !result) {
      std::cerr << "try_send_for failed when it should not have";
      return 1;
    }
  }
  {
    auto result = tx.try_send_for(4, std::chrono::microseconds(1));
    if (result) {
      std::cerr << "expected try_send_for to fail but it succeeded";
      return 1;
    }
    if (!result.error().is_full()) {
      std::cerr << "expected try_send_for error to be \"full\" but it was not";
      return 1;
    }
    if (result.error().item != 4) {
      std::cerr << "wrong item in try_send_for error. expected 4 got "
                << result.error().item;
      return 1;
    }
  }
  for (int i = 0; i < int(buffer_capacity); ++i) {
    auto result = rx.try_recv_for(std::chrono::microseconds(1));
    if (!result) {
      std::cerr << "try_recv_for failed when it should not have";
      return 1;
    }
    if (*result != i) {
      std::cerr << "wrong item from try_recv: expected " << i << " got "
                << *result;
      return 1;
    }
  }
  {
    auto result = rx.try_recv_for(std::chrono::microseconds(1));
    if (result) {
      std::cerr << "expected try_recv_for to fail but it succeeded";
      return 1;
    }
    if (!result.error().is_empty()) {
      std::cerr << "expected try_recv_for error to be \"empty\" but it was not";
      return 1;
    }
  }

  for (int i = 0; i < int(buffer_capacity); ++i) {
    if (auto result = tx.try_send_until(i, std::chrono::steady_clock::now() +
                                               std::chrono::microseconds(1));
        !result) {
      std::cerr << "try_send_until failed when it should not have";
      return 1;
    }
  }
  {
    auto result = tx.try_send_until(5, std::chrono::steady_clock::now() +
                                           std::chrono::microseconds(1));
    if (result) {
      std::cerr << "expected try_send_until to fail but it succeeded";
      return 1;
    }
    if (!result.error().is_full()) {
      std::cerr
          << "expected try_send_until error to be \"full\" but it was not";
      return 1;
    }
    if (result.error().item != 5) {
      std::cerr << "wrong item in try_send_until error. expected 5 got "
                << result.error().item;
      return 1;
    }
  }
  for (int i = 0; i < int(buffer_capacity); ++i) {
    auto result = rx.try_recv_until(std::chrono::steady_clock::now() +
                                    std::chrono::microseconds(1));
    if (!result) {
      std::cerr << "try_recv_until failed when it should not have";
      return 1;
    }
    if (*result != i) {
      std::cerr << "wrong item from try_recv_until: expected " << i << " got "
                << *result;
      return 1;
    }
  }
  {
    auto result = rx.try_recv_until(std::chrono::steady_clock::now() +
                                    std::chrono::microseconds(1));
    if (result) {
      std::cerr << "expected try_recv_until to fail but it succeeded";
      return 1;
    }
    if (!result.error().is_empty()) {
      std::cerr
          << "expected try_recv_until error to be \"empty\" but it was not";
      return 1;
    }
  }

  return 0;
}
} // namespace test_util

#endif
