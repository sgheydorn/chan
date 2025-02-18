#ifndef _TEST_UTIL_MANY_TO_MANY_DISCONNECT_SENDER
#define _TEST_UTIL_MANY_TO_MANY_DISCONNECT_SENDER

#include <iostream>
#include <map>
#include <thread>
#include <utility>
#include <vector>

namespace test_util {
template <typename S, typename R>
int many_to_many_disconnect_sender(S tx, R rx) {
  std::pair<std::thread, bool> tx_threads[10];
  for (auto &[tx_thread, send_fail] : tx_threads) {
    send_fail = false;
    tx_thread = std::thread([tx, &send_fail] mutable {
      for (int i = 0; i < 1000; ++i) {
        if (!tx.send(i)) {
          send_fail = true;
          break;
        }
      }
    });
  }
  tx.disconnect();

  std::pair<std::thread, std::vector<int>> rx_threads[10];
  for (auto &[rx_thread, items] : rx_threads) {
    rx_thread = std::thread([rx, &items]() mutable {
      for (auto item : rx) {
        items.push_back(item);
      }
    });
  }
  rx.disconnect();

  for (auto &[tx_thread, _] : tx_threads) {
    tx_thread.join();
  }
  for (auto &[rx_thread, _] : rx_threads) {
    rx_thread.join();
  }

  for (auto &[_, send_fail] : tx_threads) {
    if (send_fail) {
      std::cerr << "send failed when it should not have";
      return 1;
    }
  }

  auto total_item_count = 0zu;
  for (auto &[_, items] : rx_threads) {
    total_item_count += items.size();
  }
  if (total_item_count != 10000) {
    std::cerr << "expected 10000 items but got " << total_item_count;
    return 1;
  }

  std::map<int, int> item_counts;
  for (auto &[_, items] : rx_threads) {
    for (auto item : items) {
      ++item_counts[item];
    }
  }

  for (int i = 0; i < 1000; ++i) {
    if (item_counts[i] != 10) {
      std::cerr << "wrong item count for item " << i << ": expected 10 got "
                << item_counts[i];
      return 1;
    }
  }

  return 0;
}
} // namespace test_util

#endif
