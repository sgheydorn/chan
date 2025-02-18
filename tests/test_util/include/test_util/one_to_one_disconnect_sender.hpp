#ifndef _TEST_UTIL_ONE_TO_ONE_DISCONNECT_SENDER
#define _TEST_UTIL_ONE_TO_ONE_DISCONNECT_SENDER

#include <iostream>
#include <thread>
#include <utility>
#include <vector>

namespace test_util {
template <typename S, typename R> int one_to_one_disconnect_sender(S tx, R rx) {
  auto send_fail = false;
  auto tx_thread = std::thread([tx = std::move(tx), &send_fail] mutable {
    for (int i = 0; i < 1000; ++i) {
      if (!tx.send(i)) {
        send_fail = true;
        break;
      }
    }
  });

  std::vector<int> items;
  auto rx_thread = std::thread([rx = std::move(rx), &items]() mutable {
    for (auto item : rx) {
      items.push_back(item);
    }
  });

  tx_thread.join();
  rx_thread.join();

  if (send_fail) {
    std::cerr << "send failed when it should not have";
    return 1;
  }

  if (items.size() != 1000) {
    std::cerr << "expected 1000 items but got " << items.size();
    return 1;
  }

  for (int i = 0; i < 1000; ++i) {
    if (items[i] != i) {
      std::cerr << "wrong item at index " << i << ": expected " << i << " got "
                << items[i];
      return 1;
    }
  }

  return 0;
}
} // namespace test_util

#endif
