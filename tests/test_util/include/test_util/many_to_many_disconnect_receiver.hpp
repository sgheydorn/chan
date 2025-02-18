#ifndef _TEST_UTIL_MANY_TO_MANY_DISCONNECT_RECEIVER
#define _TEST_UTIL_MANY_TO_MANY_DISCONNECT_RECEIVER

#include <iostream>
#include <thread>
#include <utility>
#include <vector>

namespace test_util {
template <typename S, typename R>
int many_to_many_disconnect_receiver(S tx, R rx) {
  std::thread tx_threads[10];
  for (auto &tx_thread : tx_threads) {
    tx_thread = std::thread([tx] mutable {
      for (int i = 0;; ++i) {
        if (!tx.send(i)) {
          break;
        }
      }
    });
  }
  tx.disconnect();

  std::pair<std::thread, std::vector<int>> rx_threads[10];
  for (auto &[rx_thread, items] : rx_threads) {
    rx_thread = std::thread([rx, &items] mutable {
      for (int i = 0; i < 1000; ++i) {
        auto item = rx.recv();
        if (!item) {
          break;
        }
        items.push_back(*item);
      }
    });
  }
  rx.disconnect();

  for (auto &tx_thread : tx_threads) {
    tx_thread.join();
  }
  for (auto &[rx_thread, _] : rx_threads) {
    rx_thread.join();
  }

  for (auto &[_, items] : rx_threads) {
    if (items.size() != 1000) {
      std::cerr << "expected 1000 items but got " << items.size();
      return 1;
    }
  }

  return 0;
}
} // namespace test_util

#endif
