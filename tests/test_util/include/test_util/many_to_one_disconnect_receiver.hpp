#ifndef _TEST_UTIL_MANY_TO_ONE_DISCONNECT_RECEIVER
#define _TEST_UTIL_MANY_TO_ONE_DISCONNECT_RECEIVER

#include <iostream>
#include <thread>
#include <utility>
#include <vector>

namespace test_util {
template <typename S, typename R>
int many_to_one_disconnect_receiver(S tx, R rx) {
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

  std::vector<int> items;
  auto rx_thread = std::thread([rx = std::move(rx), &items]() mutable {
    for (int i = 0; i < 10000; ++i) {
      auto item = rx.recv();
      if (!item) {
        break;
      }
      items.push_back(*item);
    }
  });

  for (auto &tx_thread : tx_threads) {
    tx_thread.join();
  }
  rx_thread.join();

  if (items.size() != 10000) {
    std::cerr << "expected 10000 items but got " << items.size();
    return 1;
  }

  return 0;
}
} // namespace test_util

#endif
