#ifndef _TEST_UTIL_ADD_REMOVE_RECEIVER
#define _TEST_UTIL_ADD_REMOVE_RECEIVER

#include <iostream>
#include <thread>
#include <utility>

namespace test_util {
template <typename S, typename R> int add_remove_receiver(S tx, R rx) {
  std::pair<std::thread, bool> rx_threads[10];
  for (auto &[rx_thread, recv_fail] : rx_threads) {
    recv_fail = false;
    rx_thread = std::thread([rx, &recv_fail] {
      for (int i = 0; i < 500; ++i) {
        auto rx_copy = rx;
        for (int j = 0; j < 2; ++j) {
          if (!rx_copy.recv()) {
            recv_fail = true;
          }
        }
      }
    });
  }
  rx.disconnect();

  while (true) {
    if (!tx.send(0)) {
      break;
    }
  }

  for (auto &[rx_thread, _] : rx_threads) {
    rx_thread.join();
  }
  for (auto &[_, recv_fail] : rx_threads) {
    if (recv_fail) {
      std::cerr << "recv failed unexpectedly";
      return 1;
    }
  }

  return 0;
}
} // namespace test_util

#endif
