#ifndef _TEST_UTIL_UNBUFFERED_TRY
#define _TEST_UTIL_UNBUFFERED_TRY

#include <iostream>
#include <thread>
#include <utility>

namespace test_util {
template <typename S, typename R> int unbuffered_try(S tx, R rx) {
  {
    auto result = tx.try_send(3);
    if (result) {
      std::cerr << "try_send succeeded when it should not failed";
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
  {
    auto result = tx.try_send_for(4, std::chrono::microseconds(1));
    if (result) {
      std::cerr << "try_send_for succeeded when it should not failed";
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
  {
    auto result = tx.try_send_until(5, std::chrono::steady_clock::now() +
                                           std::chrono::microseconds(1));
    if (result) {
      std::cerr << "try_send_until succeeded when it should not failed";
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

  {
    auto result = rx.try_recv();
    if (result) {
      std::cerr << "try_recv succeeded when it should not failed";
      return 1;
    }
    if (!result.error().is_empty()) {
      std::cerr << "expected try_recv error to be \"empty\" but it was not";
      return 1;
    }
  }
  {
    auto result = rx.try_recv_for(std::chrono::microseconds(1));
    if (result) {
      std::cerr << "try_recv_for succeeded when it should not failed";
      return 1;
    }
    if (!result.error().is_empty()) {
      std::cerr << "expected try_recv_for error to be \"empty\" but it was not";
      return 1;
    }
  }
  {
    auto result = rx.try_recv_until(std::chrono::steady_clock::now() +
                                    std::chrono::microseconds(1));
    if (result) {
      std::cerr << "try_recv_until succeeded when it should not failed";
      return 1;
    }
    if (!result.error().is_empty()) {
      std::cerr
          << "expected try_recv_until error to be \"empty\" but it was not";
      return 1;
    }
  }

  auto recv_disconnected = false;
  auto recv_timeout = false;
  auto wrong_item = false;
  // `rx_thread` owns a reference to `rx` instead of owning `rx` so that we can
  // use `rx` again after joining `rx_thread`. We do not access `rx` while
  // `rx_thread` is running, so this is thread safe.
  auto rx_thread = std::thread(
      [&rx, &recv_disconnected, &recv_timeout, &wrong_item] mutable {
        for (int i = 3; i < 6; ++i) {
          auto item = rx.try_recv_for(std::chrono::seconds(1));
          if (!item) {
            if (item.error().is_disconnected()) {
              recv_disconnected = true;
            } else {
              recv_timeout = true;
            }
            break;
          }
          if (*item != i) {
            wrong_item = true;
            break;
          }
        }
      });
  {
    auto result = tx.try_send_for(3, std::chrono::seconds(1));
    if (!result) {
      rx_thread.detach();
      std::cerr << "try_send_for failed when it should not have";
      return 1;
    }
  }
  {
    auto result = tx.try_send_until(4, std::chrono::steady_clock::now() +
                                           std::chrono::seconds(1));
    if (!result) {
      rx_thread.detach();
      std::cerr << "try_send_until failed when it should not have";
      return 1;
    }
  }
  {
    // It will take time for rx_thread to begin receiving, so keep calling
    // try_send for up to a second.
    for (int i = 0;; ++i) {
      if (i == 1000) {
        rx_thread.detach();
        std::cerr << "try_send failed after 1000 attempts";
        return 1;
      }
      auto result = tx.try_send(5);
      if (result) {
        break;
      }
      if (result.error().is_disconnected()) {
        rx_thread.detach();
        std::cerr << "try_send error was \"disconnected\" when it should have "
                     "succeeded";
        return 1;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
  rx_thread.join();
  if (recv_disconnected) {
    std::cerr << "recv failed with \"disconnected\" when it should not have";
    return 1;
  }
  if (recv_timeout) {
    std::cerr << "recv timed out when it should not have";
    return 1;
  }
  if (wrong_item) {
    std::cerr << "wrong item from recv";
    return 1;
  }

  auto send_disconnected = false;
  auto send_timeout = false;
  auto tx_thread = std::thread(
      [tx = std::move(tx), &send_disconnected, &send_timeout] mutable {
        for (int i = 3; i < 6; ++i) {
          auto result = tx.try_send_for(i, std::chrono::seconds(1));
          if (!result) {
            if (result.error().is_disconnected()) {
              send_disconnected = true;
            } else {
              send_timeout = true;
            }
            break;
          }
        }
      });
  {
    auto result = rx.try_recv_for(std::chrono::seconds(1));
    if (!result) {
      tx_thread.detach();
      std::cerr << "try_recv_for failed when it should not have";
      return 1;
    }
    if (*result != 3) {
      tx_thread.detach();
      std::cerr << "wrong item from try_recv_for. expected 3 got " << *result;
      return 1;
    }
  }
  {
    auto result = rx.try_recv_until(std::chrono::steady_clock::now() +
                                    std::chrono::seconds(1));
    if (!result) {
      tx_thread.detach();
      std::cerr << "try_recv_until failed when it should not have";
      return 1;
    }
    if (*result != 4) {
      tx_thread.detach();
      std::cerr << "wrong item from try_recv_until. expected 4 got " << *result;
      return 1;
    }
  }
  {
    // It will take time for tx_thread to begin sending, so keep calling
    // try_recv for up to a second.
    for (int i = 0;; ++i) {
      if (i == 1000) {
        tx_thread.detach();
        std::cerr << "try_recv failed after 1000 attempts";
        return 1;
      }
      auto result = rx.try_recv();
      if (result) {
        if (*result != 5) {
          tx_thread.detach();
          std::cerr << "wrong item from try_recv. expected 5 got " << *result;
          return 1;
        }
        break;
      }
      if (result.error().is_disconnected()) {
        tx_thread.detach();
        std::cerr << "try_recv error was \"disconnected\" when it should have "
                     "succeeded";
        return 1;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
  tx_thread.join();
  if (send_disconnected) {
    std::cerr << "send failed with \"disconnected\" when it should not have";
    return 1;
  }
  if (send_timeout) {
    std::cerr << "send timed out when it should not have";
    return 1;
  }

  return 0;
}
} // namespace test_util

#endif
