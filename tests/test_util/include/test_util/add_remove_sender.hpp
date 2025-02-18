#ifndef _TEST_UTIL_ADD_REMOVE_SENDER
#define _TEST_UTIL_ADD_REMOVE_SENDER

#include <iostream>
#include <thread>
#include <utility>
#include <vector>

namespace test_util {
template <typename S, typename R> int add_remove_sender(S tx, R rx) {
  std::pair<std::thread, bool> tx_threads[10];
  for (auto &[tx_thread, send_fail] : tx_threads) {
    send_fail = false;
    tx_thread = std::thread([tx, &send_fail] {
      for (int i = 0; i < 500; ++i) {
        auto tx_copy = tx;
        for (int j = 0; j < 2; ++j) {
          if (!tx_copy.send(j)) {
            send_fail = true;
          }
        }
      }
    });
  }
  tx.disconnect();

  std::vector<int> items;
  for (auto item : rx) {
    items.push_back(item);
  }

  for (auto &[tx_thread, _] : tx_threads) {
    tx_thread.join();
  }
  for (auto &[_, send_fail] : tx_threads) {
    if (send_fail) {
      std::cerr << "send failed unexpectedly";
      return 1;
    }
  }

  if (items.size() != 10000) {
    std::cerr << "expected to recv 10000 items but got " << items.size();
    return 1;
  }

  return 0;
}
} // namespace test_util

#endif
