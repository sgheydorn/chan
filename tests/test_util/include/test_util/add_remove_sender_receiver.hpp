#ifndef _TEST_UTIL_ADD_REMOVE_SENDER_RECEIVER
#define _TEST_UTIL_ADD_REMOVE_SENDER_RECEIVER

#include <atomic>
#include <iostream>
#include <thread>
#include <utility>

namespace test_util {
template <typename S, typename R> int add_remove_sender_receiver(S tx, R rx) {
  std::pair<std::thread, bool> tx_threads[10];
  for (auto &[tx_thread, send_fail] : tx_threads) {
    send_fail = false;
    tx_thread = std::thread([tx, &send_fail] {
      for (int i = 0; i < 500; ++i) {
        auto tx_copy = tx;
        for (int j = 0; j < 2; ++j) {
          if (!tx_copy.send(0)) {
            send_fail = true;
          }
        }
      }
    });
  }
  tx.disconnect();

  std::atomic_int recv_count(0);

  std::thread rx_threads[10];
  for (auto &rx_thread : rx_threads) {
    rx_thread = std::thread([rx, &recv_count] {
      for (int i = 0; i < 500; ++i) {
        auto rx_copy = rx;
        for (int j = 0; j < 2; ++j) {
          if (!rx_copy.recv()) {
            return;
          }
          recv_count.fetch_add(1, std::memory_order::relaxed);
        }
      }
    });
  }

  for (auto &rx_thread : rx_threads) {
    rx_thread.join();
  }
  while (rx.recv()) {
    recv_count.fetch_add(1, std::memory_order::relaxed);
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

  if (auto count = recv_count.load(std::memory_order::relaxed);
      count != 10000) {
    std::cerr << "expected to recv 10000 items but got " << count;
    return 1;
  }

  return 0;
}
} // namespace test_util

#endif
