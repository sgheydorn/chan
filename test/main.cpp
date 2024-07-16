#include <iostream>
#include <map>
#include <ranges>
#include <set>
#include <sstream>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include <chan/mpmc/bounded/channel.hpp>
#include <chan/mpmc/unbounded/channel.hpp>
#include <chan/mpmc/unbuffered/channel.hpp>
#include <chan/mpsc/bounded/channel.hpp>
#include <chan/mpsc/unbounded/channel.hpp>
#include <chan/mpsc/unbuffered/channel.hpp>
#include <chan/spmc/bounded/channel.hpp>
#include <chan/spmc/unbounded/channel.hpp>
#include <chan/spmc/unbuffered/channel.hpp>
#include <chan/spsc/bounded/channel.hpp>
#include <chan/spsc/unbounded/channel.hpp>
#include <chan/spsc/unbuffered/channel.hpp>

// clang-format off
static_assert(std::movable<chan::spsc::bounded::Sender<int>>);
static_assert(!std::copyable<chan::spsc::bounded::Sender<int>>);
static_assert(std::ranges::output_range<chan::spsc::bounded::Sender<int>, int>);

static_assert(std::movable<chan::spsc::bounded::Receiver<int>>);
static_assert(!std::copyable<chan::spsc::bounded::Receiver<int>>);
static_assert(std::ranges::input_range<chan::spsc::bounded::Receiver<int>>);

static_assert(std::movable<chan::spsc::unbounded::Sender<int>>);
static_assert(!std::copyable<chan::spsc::unbounded::Sender<int>>);
static_assert(std::ranges::output_range<chan::spsc::unbounded::Sender<int>, int>);

static_assert(std::movable<chan::spsc::unbounded::Receiver<int>>);
static_assert(!std::copyable<chan::spsc::unbounded::Receiver<int>>);
static_assert(std::ranges::input_range<chan::spsc::unbounded::Receiver<int>>);

static_assert(std::movable<chan::spsc::unbuffered::Sender<int>>);
static_assert(!std::copyable<chan::spsc::unbuffered::Sender<int>>);
static_assert(std::ranges::output_range<chan::spsc::unbuffered::Sender<int>, int>);

static_assert(std::movable<chan::spsc::unbuffered::Receiver<int>>);
static_assert(!std::copyable<chan::spsc::unbuffered::Receiver<int>>);
static_assert(std::ranges::input_range<chan::spsc::unbuffered::Receiver<int>>);

static_assert(std::movable<chan::mpsc::bounded::Sender<int>>);
static_assert(std::copyable<chan::mpsc::bounded::Sender<int>>);
static_assert(std::ranges::output_range<chan::mpsc::bounded::Sender<int>, int>);

static_assert(std::movable<chan::mpsc::bounded::Receiver<int>>);
static_assert(!std::copyable<chan::mpsc::bounded::Receiver<int>>);
static_assert(std::ranges::input_range<chan::mpsc::bounded::Receiver<int>>);

static_assert(std::movable<chan::mpsc::unbounded::Sender<int>>);
static_assert(std::copyable<chan::mpsc::unbounded::Sender<int>>);
static_assert(std::ranges::output_range<chan::mpsc::unbounded::Sender<int>, int>);

static_assert(std::movable<chan::mpsc::unbounded::Receiver<int>>);
static_assert(!std::copyable<chan::mpsc::unbounded::Receiver<int>>);
static_assert(std::ranges::input_range<chan::mpsc::unbounded::Receiver<int>>);

static_assert(std::movable<chan::mpsc::unbuffered::Sender<int>>);
static_assert(std::copyable<chan::mpsc::unbuffered::Sender<int>>);
static_assert(std::ranges::output_range<chan::mpsc::unbuffered::Sender<int>, int>);

static_assert(std::movable<chan::mpsc::unbuffered::Receiver<int>>);
static_assert(!std::copyable<chan::mpsc::unbuffered::Receiver<int>>);
static_assert(std::ranges::input_range<chan::mpsc::unbuffered::Receiver<int>>);

static_assert(std::movable<chan::spmc::bounded::Sender<int>>);
static_assert(!std::copyable<chan::spmc::bounded::Sender<int>>);
static_assert(std::ranges::output_range<chan::spmc::bounded::Sender<int>, int>);

static_assert(std::movable<chan::spmc::bounded::Receiver<int>>);
static_assert(std::copyable<chan::spmc::bounded::Receiver<int>>);
static_assert(std::ranges::input_range<chan::spmc::bounded::Receiver<int>>);

static_assert(std::movable<chan::spmc::unbounded::Sender<int>>);
static_assert(!std::copyable<chan::spmc::unbounded::Sender<int>>);
static_assert(std::ranges::output_range<chan::spmc::unbounded::Sender<int>, int>);

static_assert(std::movable<chan::spmc::unbounded::Receiver<int>>);
static_assert(std::copyable<chan::spmc::unbounded::Receiver<int>>);
static_assert(std::ranges::input_range<chan::spmc::unbounded::Receiver<int>>);

static_assert(std::movable<chan::spmc::unbuffered::Sender<int>>);
static_assert(!std::copyable<chan::spmc::unbuffered::Sender<int>>);
static_assert(std::ranges::output_range<chan::spmc::unbuffered::Sender<int>, int>);

static_assert(std::movable<chan::spmc::unbuffered::Receiver<int>>);
static_assert(std::copyable<chan::spmc::unbuffered::Receiver<int>>);
static_assert(std::ranges::input_range<chan::spmc::unbuffered::Receiver<int>>);

static_assert(std::movable<chan::mpmc::bounded::Sender<int>>);
static_assert(std::copyable<chan::mpmc::bounded::Sender<int>>);
static_assert(std::ranges::output_range<chan::mpmc::bounded::Sender<int>, int>);

static_assert(std::movable<chan::mpmc::bounded::Receiver<int>>);
static_assert(std::copyable<chan::mpmc::bounded::Receiver<int>>);
static_assert(std::ranges::input_range<chan::mpmc::bounded::Receiver<int>>);

static_assert(std::movable<chan::mpmc::unbounded::Sender<int>>);
static_assert(std::copyable<chan::mpmc::unbounded::Sender<int>>);
static_assert(std::ranges::output_range<chan::mpmc::unbounded::Sender<int>, int>);

static_assert(std::movable<chan::mpmc::unbounded::Receiver<int>>);
static_assert(std::copyable<chan::mpmc::unbounded::Receiver<int>>);
static_assert(std::ranges::input_range<chan::mpmc::unbounded::Receiver<int>>);

static_assert(std::movable<chan::mpmc::unbuffered::Sender<int>>);
static_assert(std::copyable<chan::mpmc::unbuffered::Sender<int>>);
static_assert(std::ranges::output_range<chan::mpmc::unbuffered::Sender<int>, int>);

static_assert(std::movable<chan::mpmc::unbuffered::Receiver<int>>);
static_assert(std::copyable<chan::mpmc::unbuffered::Receiver<int>>);
static_assert(std::ranges::input_range<chan::mpmc::unbuffered::Receiver<int>>);
// clang-format on

template <typename S, typename R> void disconnect_sender(S tx, R rx) {
  tx.disconnect();

  if (auto item = rx.recv(); item) {
    std::ostringstream os;
    os << "expected recv to fail but it succeeded with item " << *item;
    throw std::runtime_error(std::move(os).str());
  }

  {
    auto item = rx.try_recv();
    if (item) {
      std::ostringstream os;
      os << "expected try_recv to fail but it succeeded with item " << *item;
      throw std::runtime_error(std::move(os).str());
    }

    if (!item.error().is_disconnected()) {
      throw std::runtime_error(
          "expected error from try_recv to be \"disconnected\" but it was not");
    }
  }

  {
    auto item = rx.try_recv_for(std::chrono::microseconds(1));
    if (item) {
      std::ostringstream os;
      os << "expected try_recv_for to fail but it succeeded with item "
         << *item;
      throw std::runtime_error(std::move(os).str());
    }

    if (!item.error().is_disconnected()) {
      throw std::runtime_error("expected error from try_recv_for to be "
                               "\"disconnected\" but it was not");
    }
  }

  {
    auto item = rx.try_recv_until(std::chrono::steady_clock::now() +
                                  std::chrono::microseconds(1));
    if (item) {
      std::ostringstream os;
      os << "expected try_recv_until to fail but it succeeded with item "
         << *item;
      throw std::runtime_error(std::move(os).str());
    }

    if (!item.error().is_disconnected()) {
      throw std::runtime_error("expected error from try_until to be "
                               "\"disconnected\" but it was not");
    }
  }
}

template <typename S, typename R>
void unbuffered_disconnect_sender(S tx, R rx) {
  tx.disconnect();

  if (auto item = rx.recv(); item) {
    std::ostringstream os;
    os << "expected recv to fail but it succeeded with item " << *item;
    throw std::runtime_error(std::move(os).str());
  }
}

template <typename S, typename R> void disconnect_receiver(S tx, R rx) {
  rx.disconnect();

  {
    auto result = tx.send(7);

    if (result) {
      throw std::runtime_error("expected send to fail but it succeeded");
    }

    if (result.error().item != 7) {
      std::ostringstream os;
      os << "wrong item in send error: expected 7 got " << result.error().item;
      throw std::runtime_error(std::move(os).str());
    }
  }

  {
    auto result = tx.try_send(7);

    if (result) {
      throw std::runtime_error("expected try_send to fail but it succeeded");
    }

    if (result.error().item != 7) {
      std::ostringstream os;
      os << "wrong item in try_send error: expected 7 got "
         << result.error().item;
      throw std::runtime_error(std::move(os).str());
    }

    if (!result.error().is_disconnected()) {
      throw std::runtime_error(
          "expected error from try_send to be \"disconnected\" but it was not");
    }
  }

  {
    auto result = tx.try_send_for(7, std::chrono::microseconds(1));

    if (result) {
      throw std::runtime_error(
          "expected try_send_for to fail but it succeeded");
    }

    if (result.error().item != 7) {
      std::ostringstream os;
      os << "wrong item in try_send_for error: expected 7 got "
         << result.error().item;
      throw std::runtime_error(std::move(os).str());
    }

    if (!result.error().is_disconnected()) {
      throw std::runtime_error("expected error from try_send_for to be "
                               "\"disconnected\" but it was not");
    }
  }

  {
    auto result = tx.try_send_until(7, std::chrono::steady_clock::now() +
                                           std::chrono::microseconds(1));

    if (result) {
      throw std::runtime_error(
          "expected try_send_until to fail but it succeeded");
    }

    if (result.error().item != 7) {
      std::ostringstream os;
      os << "wrong item in try_send_until error: expected 7 got "
         << result.error().item;
      throw std::runtime_error(std::move(os).str());
    }

    if (!result.error().is_disconnected()) {
      throw std::runtime_error("expected error from try_send_until to be "
                               "\"disconnected\" but it was not");
    }
  }
}

template <typename S, typename R>
void unbounded_disconnect_receiver(S tx, R rx) {
  rx.disconnect();

  {
    auto result = tx.send(7);

    if (result) {
      throw std::runtime_error("expected send to fail but it succeeded");
    }

    if (result.error().item != 7) {
      std::ostringstream os;
      os << "wrong item in send error: expected 7 got " << result.error().item;
      throw std::runtime_error(std::move(os).str());
    }
  }
}

template <typename S, typename R> void one_item(S tx, R rx) {
  if (tx.channel_capacity() != 16) {
    std::ostringstream os;
    os << "expected channel capacity (from sender) to be 16 but it is "
       << tx.channel_capacity();
    throw std::runtime_error(std::move(os).str());
  }

  if (rx.channel_capacity() != 16) {
    std::ostringstream os;
    os << "expected channel capacity (from receiver) to be 16 but it is "
       << rx.channel_capacity();
    throw std::runtime_error(std::move(os).str());
  }

  if (!tx.send(7)) {
    throw std::runtime_error("sender disconnected when it should not be");
  }

  if (tx.channel_size() != 1) {
    std::ostringstream os;
    os << "expected channel size (from sender) to be 1 but it is "
       << tx.channel_size();
    throw std::runtime_error(std::move(os).str());
  }

  auto item = rx.recv();
  if (!item) {
    throw std::runtime_error("receiver disconnected when it should not be");
  }

  if (rx.channel_size() != 0) {
    std::ostringstream os;
    os << "expected channel size (from receiver) to be 0 but it is "
       << rx.channel_size();
    throw std::runtime_error(std::move(os).str());
  }

  if (*item != 7) {
    std::ostringstream os;
    os << "wrong item from recv: expected 7 got " << *item;
    throw std::runtime_error(std::move(os).str());
  }
}

template <typename S, typename R> void two_seperate(S tx, R rx) {
  if (!tx.send(7)) {
    throw std::runtime_error("sender disconnected when it should not be");
  }

  auto item1 = rx.recv();
  if (!item1) {
    throw std::runtime_error("receiver disconnected when it should not be");
  }

  if (*item1 != 7) {
    std::ostringstream os;
    os << "wrong item from recv: expected 7 got " << *item1;
    throw std::runtime_error(std::move(os).str());
  }

  if (!tx.send(13)) {
    throw std::runtime_error("sender disconnected when it should not be");
  }

  auto item2 = rx.recv();
  if (!item2) {
    throw std::runtime_error("receiver disconnected when it should not be");
  }

  if (*item2 != 13) {
    std::ostringstream os;
    os << "wrong item from recv: expected 13 got " << *item2;
    throw std::runtime_error(std::move(os).str());
  }
}

template <typename S, typename R> void two_consecutive(S tx, R rx) {
  if (!tx.send(7)) {
    throw std::runtime_error("sender disconnected when it should not be");
  }

  if (!tx.send(13)) {
    throw std::runtime_error("sender disconnected when it should not be");
  }

  auto item1 = rx.recv();
  if (!item1) {
    throw std::runtime_error("receiver disconnected when it should not be");
  }

  if (*item1 != 7) {
    std::ostringstream os;
    os << "wrong item from recv: expected 7 got " << *item1;
    throw std::runtime_error(std::move(os).str());
  }

  auto item2 = rx.recv();
  if (!item2) {
    throw std::runtime_error("receiver disconnected when it should not be");
  }

  if (*item2 != 13) {
    std::ostringstream os;
    os << "wrong item from recv: expected 13 got " << *item2;
    throw std::runtime_error(std::move(os).str());
  }
}

template <typename S, typename R> void leftover(S tx, R rx) {
  for (int i = 0; i < 7; ++i) {
    if (!tx.send(std::make_unique<int>(i))) {
      throw std::runtime_error("send failed when it should not have");
    }
  }
}

template <typename S, typename R>
void bounded_try(S tx, R rx, std::size_t buffer_capacity) {
  for (int i = 0; i < int(buffer_capacity); ++i) {
    // Since try_send can spuriously return "full", try many times to make the
    // test consistent.
    for (int j = 0;; ++j) {
      if (j == 1000) {
        throw std::runtime_error("try_send failed after 1000 attempts");
      }
      auto result = tx.try_send(i);
      if (result) {
        break;
      }
      if (result.error().is_disconnected()) {
        throw std::runtime_error("try_send error was \"disconnected\" when it "
                                 "should have succeeded");
      }
    }
  }
  {
    auto result = tx.try_send(3);
    if (result) {
      throw std::runtime_error("expected try_send to fail but it succeeded");
    }
    if (!result.error().is_full()) {
      throw std::runtime_error(
          "expected try_send error to be \"full\" but it was not");
    }
    if (result.error().item != 3) {
      std::ostringstream os;
      os << "wrong item in try_send error. expected 3 got "
         << result.error().item;
      throw std::runtime_error(std::move(os).str());
    }
  }
  for (int i = 0; i < int(buffer_capacity); ++i) {
    // Since try_recv can spuriously return "empty", try many times to make the
    // test consistent.
    int item;
    for (int j = 0;; ++j) {
      if (j == 1000) {
        throw std::runtime_error("try_recv failed after 1000 attempts");
      }
      auto result = rx.try_recv();
      if (result) {
        item = *result;
        break;
      }
      if (result.error().is_disconnected()) {
        throw std::runtime_error("try_recv error was \"disconnected\" when it "
                                 "should have succeeded");
      }
    }
    if (item != i) {
      std::ostringstream os;
      os << "wrong item from try_recv: expected " << i << " got " << item;
      throw std::runtime_error(std::move(os).str());
    }
  }
  {
    auto result = rx.try_recv();
    if (result) {
      throw std::runtime_error("expected try_recv to fail but it succeeded");
    }
    if (!result.error().is_empty()) {
      throw std::runtime_error(
          "expected try_recv error to be \"empty\" but it was not");
    }
  }

  for (int i = 0; i < int(buffer_capacity); ++i) {
    if (auto result = tx.try_send_for(i, std::chrono::microseconds(1));
        !result) {
      throw std::runtime_error("try_send_for failed when it should not have");
    }
  }
  {
    auto result = tx.try_send_for(4, std::chrono::microseconds(1));
    if (result) {
      throw std::runtime_error(
          "expected try_send_for to fail but it succeeded");
    }
    if (!result.error().is_full()) {
      throw std::runtime_error(
          "expected try_send_for error to be \"full\" but it was not");
    }
    if (result.error().item != 4) {
      std::ostringstream os;
      os << "wrong item in try_send_for error. expected 4 got "
         << result.error().item;
      throw std::runtime_error(std::move(os).str());
    }
  }
  for (int i = 0; i < int(buffer_capacity); ++i) {
    auto result = rx.try_recv_for(std::chrono::microseconds(1));
    if (!result) {
      throw std::runtime_error("try_recv_for failed when it should not have");
    }
    if (*result != i) {
      std::ostringstream os;
      os << "wrong item from try_recv: expected " << i << " got " << *result;
      throw std::runtime_error(std::move(os).str());
    }
  }
  {
    auto result = rx.try_recv_for(std::chrono::microseconds(1));
    if (result) {
      throw std::runtime_error(
          "expected try_recv_for to fail but it succeeded");
    }
    if (!result.error().is_empty()) {
      throw std::runtime_error(
          "expected try_recv_for error to be \"empty\" but it was not");
    }
  }

  for (int i = 0; i < int(buffer_capacity); ++i) {
    if (auto result = tx.try_send_until(i, std::chrono::steady_clock::now() +
                                               std::chrono::microseconds(1));
        !result) {
      throw std::runtime_error("try_send_until failed when it should not have");
    }
  }
  {
    auto result = tx.try_send_until(5, std::chrono::steady_clock::now() +
                                           std::chrono::microseconds(1));
    if (result) {
      throw std::runtime_error(
          "expected try_send_until to fail but it succeeded");
    }
    if (!result.error().is_full()) {
      throw std::runtime_error(
          "expected try_send_until error to be \"full\" but it was not");
    }
    if (result.error().item != 5) {
      std::ostringstream os;
      os << "wrong item in try_send_until error. expected 5 got "
         << result.error().item;
      throw std::runtime_error(std::move(os).str());
    }
  }
  for (int i = 0; i < int(buffer_capacity); ++i) {
    auto result = rx.try_recv_until(std::chrono::steady_clock::now() +
                                    std::chrono::microseconds(1));
    if (!result) {
      throw std::runtime_error("try_recv_until failed when it should not have");
    }
    if (*result != i) {
      std::ostringstream os;
      os << "wrong item from try_recv_until: expected " << i << " got "
         << *result;
      throw std::runtime_error(std::move(os).str());
    }
  }
  {
    auto result = rx.try_recv_until(std::chrono::steady_clock::now() +
                                    std::chrono::microseconds(1));
    if (result) {
      throw std::runtime_error(
          "expected try_recv_until to fail but it succeeded");
    }
    if (!result.error().is_empty()) {
      throw std::runtime_error(
          "expected try_recv_until error to be \"empty\" but it was not");
    }
  }
}

template <typename S, typename R> void unbounded_try(S tx, R rx) {
  {
    auto result = rx.try_recv();
    if (result) {
      throw std::runtime_error("try_recv succeeded when it should have failed");
    }
    if (!result.error().is_empty()) {
      throw std::runtime_error(
          "expected try_recv error to be \"empty\" but it was not");
    }
  }
  {
    tx.send(3);
    auto result = rx.try_recv();
    if (!result) {
      throw std::runtime_error("try_recv failed when it should not have");
    }
    if (*result != 3) {
      std::ostringstream os;
      os << "wrong item from try_recv: expected 3 got " << *result;
      throw std::runtime_error(std::move(os).str());
    }
  }
  {
    auto result = rx.try_recv_for(std::chrono::microseconds(1));
    if (result) {
      throw std::runtime_error(
          "try_recv_for succeeded when it should have failed");
    }
    if (!result.error().is_empty()) {
      throw std::runtime_error(
          "expected try_recv_for error to be \"empty\" but it was not");
    }
  }
  {
    tx.send(4);
    auto result = rx.try_recv_for(std::chrono::microseconds(1));
    if (!result) {
      throw std::runtime_error("try_recv_for failed when it should not have");
    }
    if (*result != 4) {
      std::ostringstream os;
      os << "wrong item from try_recv_for: expected 4 got " << *result;
      throw std::runtime_error(std::move(os).str());
    }
  }
  {
    auto result = rx.try_recv_until(std::chrono::steady_clock::now() +
                                    std::chrono::microseconds(1));
    if (result) {
      throw std::runtime_error(
          "try_recv_until succeeded when it should have failed");
    }
    if (!result.error().is_empty()) {
      throw std::runtime_error(
          "expected try_recv_until error to be \"empty\" but it was not");
    }
  }
  {
    tx.send(5);
    auto result = rx.try_recv_until(std::chrono::steady_clock::now() +
                                    std::chrono::microseconds(1));
    if (!result) {
      throw std::runtime_error("try_recv_until failed when it should not have");
    }
    if (*result != 5) {
      std::ostringstream os;
      os << "wrong item from try_recv_until: expected 5 got " << *result;
      throw std::runtime_error(std::move(os).str());
    }
  }
}

template <typename S, typename R> void unbuffered_try(S tx, R rx) {
  {
    auto result = tx.try_send(3);
    if (result) {
      throw std::runtime_error("try_send succeeded when it should not failed");
    }
    if (!result.error().is_full()) {
      throw std::runtime_error(
          "expected try_send error to be \"full\" but it was not");
    }
    if (result.error().item != 3) {
      std::ostringstream os;
      os << "wrong item in try_send error. expected 3 got "
         << result.error().item;
      throw std::runtime_error(std::move(os).str());
    }
  }
  {
    auto result = tx.try_send_for(4, std::chrono::microseconds(1));
    if (result) {
      throw std::runtime_error(
          "try_send_for succeeded when it should not failed");
    }
    if (!result.error().is_full()) {
      throw std::runtime_error(
          "expected try_send_for error to be \"full\" but it was not");
    }
    if (result.error().item != 4) {
      std::ostringstream os;
      os << "wrong item in try_send_for error. expected 4 got "
         << result.error().item;
      throw std::runtime_error(std::move(os).str());
    }
  }
  {
    auto result = tx.try_send_until(5, std::chrono::steady_clock::now() +
                                           std::chrono::microseconds(1));
    if (result) {
      throw std::runtime_error(
          "try_send_until succeeded when it should not failed");
    }
    if (!result.error().is_full()) {
      throw std::runtime_error(
          "expected try_send_until error to be \"full\" but it was not");
    }
    if (result.error().item != 5) {
      std::ostringstream os;
      os << "wrong item in try_send_until error. expected 5 got "
         << result.error().item;
      throw std::runtime_error(std::move(os).str());
    }
  }

  {
    auto result = rx.try_recv();
    if (result) {
      throw std::runtime_error("try_recv succeeded when it should not failed");
    }
    if (!result.error().is_empty()) {
      throw std::runtime_error(
          "expected try_recv error to be \"empty\" but it was not");
    }
  }
  {
    auto result = rx.try_recv_for(std::chrono::microseconds(1));
    if (result) {
      throw std::runtime_error(
          "try_recv_for succeeded when it should not failed");
    }
    if (!result.error().is_empty()) {
      throw std::runtime_error(
          "expected try_recv_for error to be \"empty\" but it was not");
    }
  }
  {
    auto result = rx.try_recv_until(std::chrono::steady_clock::now() +
                                    std::chrono::microseconds(1));
    if (result) {
      throw std::runtime_error(
          "try_recv_until succeeded when it should not failed");
    }
    if (!result.error().is_empty()) {
      throw std::runtime_error(
          "expected try_recv_until error to be \"empty\" but it was not");
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
      throw std::runtime_error("try_send_for failed when it should not have");
    }
  }
  {
    auto result = tx.try_send_until(4, std::chrono::steady_clock::now() +
                                           std::chrono::seconds(1));
    if (!result) {
      rx_thread.detach();
      throw std::runtime_error("try_send_until failed when it should not have");
    }
  }
  {
    // It will take time for rx_thread to begin receiving, so keep calling
    // try_send for up to a second.
    for (int i = 0;; ++i) {
      if (i == 1000) {
        rx_thread.detach();
        throw std::runtime_error("try_send failed after 1000 attempts");
      }
      auto result = tx.try_send(5);
      if (result) {
        break;
      }
      if (result.error().is_disconnected()) {
        rx_thread.detach();
        throw std::runtime_error("try_send error was \"disconnected\" when it "
                                 "should have succeeded");
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
  rx_thread.join();
  if (recv_disconnected) {
    throw std::runtime_error(
        "recv failed with \"disconnected\" when it should not have");
  }
  if (recv_timeout) {
    throw std::runtime_error("recv timed out when it should not have");
  }
  if (wrong_item) {
    throw std::runtime_error("wrong item from recv");
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
      throw std::runtime_error("try_recv_for failed when it should not have");
    }
    if (*result != 3) {
      tx_thread.detach();
      std::ostringstream os;
      os << "wrong item from try_recv_for. expected 3 got " << *result;
      throw std::runtime_error(std::move(os).str());
    }
  }
  {
    auto result = rx.try_recv_until(std::chrono::steady_clock::now() +
                                    std::chrono::seconds(1));
    if (!result) {
      tx_thread.detach();
      throw std::runtime_error("try_recv_until failed when it should not have");
    }
    if (*result != 4) {
      tx_thread.detach();
      std::ostringstream os;
      os << "wrong item from try_recv_until. expected 4 got " << *result;
      throw std::runtime_error(std::move(os).str());
    }
  }
  {
    // It will take time for tx_thread to begin sending, so keep calling
    // try_recv for up to a second.
    for (int i = 0;; ++i) {
      if (i == 1000) {
        tx_thread.detach();
        throw std::runtime_error("try_recv failed after 1000 attempts");
      }
      auto result = rx.try_recv();
      if (result) {
        if (*result != 5) {
          tx_thread.detach();
          std::ostringstream os;
          os << "wrong item from try_recv. expected 5 got " << *result;
          throw std::runtime_error(std::move(os).str());
        }
        break;
      }
      if (result.error().is_disconnected()) {
        tx_thread.detach();
        throw std::runtime_error("try_recv error was \"disconnected\" when it "
                                 "should have succeeded");
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
  tx_thread.join();
  if (send_disconnected) {
    throw std::runtime_error(
        "send failed with \"disconnected\" when it should not have");
  }
  if (send_timeout) {
    throw std::runtime_error("send timed out when it should not have");
  }
}

template <typename S, typename R>
void one_to_one_disconnect_sender(S tx, R rx) {
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
    throw std::runtime_error("send failed when it should not have");
  }

  if (items.size() != 1000) {
    std::ostringstream os;
    os << "expected 1000 items but got " << items.size();
    throw std::runtime_error(std::move(os).str());
  }

  for (int i = 0; i < 1000; ++i) {
    if (items[i] != i) {
      std::ostringstream os;
      os << "wrong item at index " << i << ": expected " << i << " got "
         << items[i];
      throw std::runtime_error(std::move(os).str());
    }
  }
}

template <typename S, typename R>
void one_to_one_disconnect_receiver(S tx, R rx) {
  auto tx_thread = std::thread([tx = std::move(tx)] mutable {
    for (int i = 0;; ++i) {
      if (!tx.send(i)) {
        break;
      }
    }
  });

  std::vector<int> items;
  auto rx_thread = std::thread([rx = std::move(rx), &items]() mutable {
    for (int i = 0; i < 1000; ++i) {
      auto item = rx.recv();
      if (!item) {
        break;
      }
      items.push_back(*item);
    }
  });

  tx_thread.join();
  rx_thread.join();

  if (items.size() != 1000) {
    std::ostringstream os;
    os << "expected 1000 items but got " << items.size();
    throw std::runtime_error(std::move(os).str());
  }

  for (int i = 0; i < 1000; ++i) {
    if (items[i] != i) {
      std::ostringstream os;
      os << "wrong item at index " << i << ": expected " << i << " got "
         << items[i];
      throw std::runtime_error(std::move(os).str());
    }
  }
}

template <typename S, typename R>
void many_to_one_disconnect_sender(S tx, R rx) {
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

  std::vector<int> items;
  auto rx_thread = std::thread([rx = std::move(rx), &items]() mutable {
    for (auto item : rx) {
      items.push_back(item);
    }
  });

  for (auto &[tx_thread, _] : tx_threads) {
    tx_thread.join();
  }
  rx_thread.join();

  for (auto &[_, send_fail] : tx_threads) {
    if (send_fail) {
      throw std::runtime_error("send failed when it should not have");
    }
  }

  if (items.size() != 10000) {
    std::ostringstream os;
    os << "expected 10000 items but got " << items.size();
    throw std::runtime_error(std::move(os).str());
  }

  std::map<int, int> item_counts;
  for (auto item : items) {
    ++item_counts[item];
  }

  for (int i = 0; i < 1000; ++i) {
    if (item_counts[i] != 10) {
      std::ostringstream os;
      os << "wrong item count for item " << i << ": expected 10 got "
         << item_counts[i];
      throw std::runtime_error(std::move(os).str());
    }
  }
}

template <typename S, typename R>
void many_to_one_disconnect_receiver(S tx, R rx) {
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
    std::ostringstream os;
    os << "expected 10000 items but got " << items.size();
    throw std::runtime_error(std::move(os).str());
  }
}

template <typename S, typename R>
void one_to_many_disconnect_sender(S tx, R rx) {
  auto send_fail = false;
  auto tx_thread = std::thread([tx = std::move(tx), &send_fail] mutable {
    for (int i = 0; i < 10000; ++i) {
      if (!tx.send(i)) {
        send_fail = true;
        break;
      }
    }
  });

  std::pair<std::thread, std::vector<int>> rx_threads[10];
  for (auto &[rx_thread, items] : rx_threads) {
    rx_thread = std::thread([rx, &items]() mutable {
      for (auto item : rx) {
        items.push_back(item);
      }
    });
  }
  rx.disconnect();

  tx_thread.join();
  for (auto &[rx_thread, _] : rx_threads) {
    rx_thread.join();
  }

  if (send_fail) {
    throw std::runtime_error("send failed when it should not have");
  }

  auto total_item_count = 0zu;
  for (auto &[_, items] : rx_threads) {
    total_item_count += items.size();
  }
  if (total_item_count != 10000) {
    std::ostringstream os;
    os << "expected 10000 items but got " << total_item_count;
    throw std::runtime_error(std::move(os).str());
  }

  std::map<int, int> item_counts;
  for (auto &[_, items] : rx_threads) {
    for (auto item : items) {
      ++item_counts[item];
    }
  }

  for (int i = 0; i < 10000; ++i) {
    if (item_counts[i] != 1) {
      std::ostringstream os;
      os << "wrong item count for item " << i << ": expected 1 got "
         << item_counts[i];
      throw std::runtime_error(std::move(os).str());
    }
  }
}

template <typename S, typename R>
void one_to_many_disconnect_receiver(S tx, R rx) {
  auto tx_thread = std::thread([tx = std::move(tx)] mutable {
    for (int i = 0;; ++i) {
      if (!tx.send(i)) {
        break;
      }
    }
  });

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

  tx_thread.join();
  for (auto &[rx_thread, _] : rx_threads) {
    rx_thread.join();
  }

  for (auto &[_, items] : rx_threads) {
    if (items.size() != 1000) {
      std::ostringstream os;
      os << "expected 1000 items but got " << items.size();
      throw std::runtime_error(std::move(os).str());
    }
  }
}

template <typename S, typename R>
void many_to_many_disconnect_sender(S tx, R rx) {
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
      throw std::runtime_error("send failed when it should not have");
    }
  }

  auto total_item_count = 0zu;
  for (auto &[_, items] : rx_threads) {
    total_item_count += items.size();
  }
  if (total_item_count != 10000) {
    std::ostringstream os;
    os << "expected 10000 items but got " << total_item_count;
    throw std::runtime_error(std::move(os).str());
  }

  std::map<int, int> item_counts;
  for (auto &[_, items] : rx_threads) {
    for (auto item : items) {
      ++item_counts[item];
    }
  }

  for (int i = 0; i < 1000; ++i) {
    if (item_counts[i] != 10) {
      std::ostringstream os;
      os << "wrong item count for item " << i << ": expected 10 got "
         << item_counts[i];
      throw std::runtime_error(std::move(os).str());
    }
  }
}

template <typename S, typename R>
void many_to_many_disconnect_receiver(S tx, R rx) {
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
      std::ostringstream os;
      os << "expected 1000 items but got " << items.size();
      throw std::runtime_error(std::move(os).str());
    }
  }
}

template <typename S, typename R> void add_remove_sender(S tx, R rx) {
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
      throw std::runtime_error("send failed unexpectedly");
    }
  }

  if (items.size() != 10000) {
    std::ostringstream os;
    os << "expected to recv 10000 items but got " << items.size();
    throw std::runtime_error(std::move(os).str());
  }
}

template <typename S, typename R> void add_remove_receiver(S tx, R rx) {
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
      throw std::runtime_error("recv failed unexpectedly");
    }
  }
}

template <typename S, typename R> void add_remove_sender_receiver(S tx, R rx) {
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
      throw std::runtime_error("send failed unexpectedly");
    }
  }

  if (auto count = recv_count.load(std::memory_order::relaxed);
      count != 10000) {
    std::ostringstream os;
    os << "expected to recv 10000 items but got " << count;
    throw std::runtime_error(std::move(os).str());
  }
}

void spsc_bounded_disconnect_sender() {
  auto [tx, rx] = chan::spsc::bounded::channel<int>(16);
  disconnect_sender(std::move(tx), std::move(rx));
}

void spsc_bounded_disconnect_receiver() {
  auto [tx, rx] = chan::spsc::bounded::channel<int>(16);
  disconnect_receiver(std::move(tx), std::move(rx));
}

void spsc_bounded_one_item() {
  auto [tx, rx] = chan::spsc::bounded::channel<int>(16);
  one_item(std::move(tx), std::move(rx));
}

void spsc_bounded_two_seperate() {
  auto [tx, rx] = chan::spsc::bounded::channel<int>(16);
  two_seperate(std::move(tx), std::move(rx));
}

void spsc_bounded_two_consecutive() {
  auto [tx, rx] = chan::spsc::bounded::channel<int>(16);
  two_consecutive(std::move(tx), std::move(rx));
}

void spsc_bounded_leftover() {
  auto [tx, rx] = chan::spsc::bounded::channel<std::unique_ptr<int>>(16);
  leftover(std::move(tx), std::move(rx));
}

void spsc_bounded_try() {
  auto [tx, rx] = chan::spsc::bounded::channel<int>(16);
  bounded_try(std::move(tx), std::move(rx), 16);
}

void spsc_bounded_one_to_one_disconnect_sender() {
  auto [tx, rx] = chan::spsc::bounded::channel<int>(16);
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void spsc_bounded_one_to_one_disconnect_sender_buffer_size_1() {
  auto [tx, rx] = chan::spsc::bounded::channel<int>(1);
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void spsc_bounded_one_to_one_disconnect_receiver() {
  auto [tx, rx] = chan::spsc::bounded::channel<int>(16);
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void spsc_bounded_one_to_one_disconnect_receiver_buffer_size_1() {
  auto [tx, rx] = chan::spsc::bounded::channel<int>(1);
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void spsc_unbounded_disconnect_sender() {
  auto [tx, rx] = chan::spsc::unbounded::channel<int>();
  disconnect_sender(std::move(tx), std::move(rx));
}

void spsc_unbounded_disconnect_receiver() {
  auto [tx, rx] = chan::spsc::unbounded::channel<int>();
  unbounded_disconnect_receiver(std::move(tx), std::move(rx));
}

void spsc_unbounded_one_item() {
  auto [tx, rx] = chan::spsc::unbounded::channel<int>();
  one_item(std::move(tx), std::move(rx));
}

void spsc_unbounded_two_seperate() {
  auto [tx, rx] = chan::spsc::unbounded::channel<int>();
  two_seperate(std::move(tx), std::move(rx));
}

void spsc_unbounded_two_consecutive() {
  auto [tx, rx] = chan::spsc::unbounded::channel<int>();
  two_consecutive(std::move(tx), std::move(rx));
}

void spsc_unbounded_leftover() {
  auto [tx, rx] = chan::spsc::unbounded::channel<std::unique_ptr<int>, 2>();
  leftover(std::move(tx), std::move(rx));
}

void spsc_unbounded_try() {
  auto [tx, rx] = chan::spsc::unbounded::channel<int>();
  unbounded_try(std::move(tx), std::move(rx));
}

void spsc_unbounded_one_to_one_disconnect_sender() {
  auto [tx, rx] = chan::spsc::unbounded::channel<int>();
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void spsc_unbounded_one_to_one_disconnect_sender_chunk_size_1() {
  auto [tx, rx] = chan::spsc::unbounded::channel<int, 1>();
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void spsc_unbounded_one_to_one_disconnect_receiver() {
  auto [tx, rx] = chan::spsc::unbounded::channel<int>();
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void spsc_unbounded_one_to_one_disconnect_receiver_chunk_size_1() {
  auto [tx, rx] = chan::spsc::unbounded::channel<int, 1>();
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void spsc_unbuffered_disconnect_sender() {
  auto [tx, rx] = chan::spsc::unbuffered::channel<int>();
  unbuffered_disconnect_sender(std::move(tx), std::move(rx));
}

void spsc_unbuffered_disconnect_receiver() {
  auto [tx, rx] = chan::spsc::unbuffered::channel<int>();
  disconnect_receiver(std::move(tx), std::move(rx));
}

void spsc_unbuffered_try() {
  auto [tx, rx] = chan::spsc::unbuffered::channel<int>();
  unbuffered_try(std::move(tx), std::move(rx));
}

void spsc_unbuffered_one_to_one_disconnect_sender() {
  auto [tx, rx] = chan::spsc::unbuffered::channel<int>();
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void spsc_unbuffered_one_to_one_disconnect_receiver() {
  auto [tx, rx] = chan::spsc::unbuffered::channel<int>();
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpsc_bounded_disconnect_sender() {
  auto [tx, rx] = chan::mpsc::bounded::channel<int>(16);
  disconnect_sender(std::move(tx), std::move(rx));
}

void mpsc_bounded_disconnect_receiver() {
  auto [tx, rx] = chan::mpsc::bounded::channel<int>(16);
  disconnect_receiver(std::move(tx), std::move(rx));
}

void mpsc_bounded_one_item() {
  auto [tx, rx] = chan::mpsc::bounded::channel<int>(16);
  one_item(std::move(tx), std::move(rx));
}

void mpsc_bounded_two_seperate() {
  auto [tx, rx] = chan::mpsc::bounded::channel<int>(16);
  two_seperate(std::move(tx), std::move(rx));
}

void mpsc_bounded_two_consecutive() {
  auto [tx, rx] = chan::mpsc::bounded::channel<int>(16);
  two_consecutive(std::move(tx), std::move(rx));
}

void mpsc_bounded_leftover() {
  auto [tx, rx] = chan::mpsc::bounded::channel<std::unique_ptr<int>>(16);
  leftover(std::move(tx), std::move(rx));
}

void mpsc_bounded_try() {
  auto [tx, rx] = chan::mpsc::bounded::channel<int>(16);
  bounded_try(std::move(tx), std::move(rx), 16);
}

void mpsc_bounded_one_to_one_disconnect_sender() {
  auto [tx, rx] = chan::mpsc::bounded::channel<int>(16);
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpsc_bounded_one_to_one_disconnect_sender_buffer_size_1() {
  auto [tx, rx] = chan::mpsc::bounded::channel<int>(1);
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpsc_bounded_one_to_one_disconnect_receiver() {
  auto [tx, rx] = chan::mpsc::bounded::channel<int>(16);
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpsc_bounded_one_to_one_disconnect_receiver_buffer_size_1() {
  auto [tx, rx] = chan::mpsc::bounded::channel<int>(1);
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpsc_bounded_many_to_one_disconnect_sender() {
  auto [tx, rx] = chan::mpsc::bounded::channel<int>(16);
  many_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpsc_bounded_many_to_one_disconnect_sender_buffer_size_1() {
  auto [tx, rx] = chan::mpsc::bounded::channel<int>(1);
  many_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpsc_bounded_many_to_one_disconnect_receiver() {
  auto [tx, rx] = chan::mpsc::bounded::channel<int>(16);
  many_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpsc_bounded_many_to_one_disconnect_receiver_buffer_size_1() {
  auto [tx, rx] = chan::mpsc::bounded::channel<int>(1);
  many_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpsc_bounded_add_remove_sender() {
  auto [tx, rx] = chan::mpsc::bounded::channel<int>(1);
  add_remove_sender(std::move(tx), std::move(rx));
}

void mpsc_unbounded_disconnect_sender() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int>();
  disconnect_sender(std::move(tx), std::move(rx));
}

void mpsc_unbounded_disconnect_receiver() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int>();
  unbounded_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpsc_unbounded_one_item() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int>();
  one_item(std::move(tx), std::move(rx));
}

void mpsc_unbounded_two_seperate() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int>();
  two_seperate(std::move(tx), std::move(rx));
}

void mpsc_unbounded_two_consecutive() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int>();
  two_consecutive(std::move(tx), std::move(rx));
}

void mpsc_unbounded_leftover() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<std::unique_ptr<int>, 2>();
  leftover(std::move(tx), std::move(rx));
}

void mpsc_unbounded_try() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int>();
  unbounded_try(std::move(tx), std::move(rx));
}

void mpsc_unbounded_one_to_one_disconnect_sender() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int>();
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpsc_unbounded_one_to_one_disconnect_sender_chunk_size_1() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int, 1>();
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpsc_unbounded_one_to_one_disconnect_receiver() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int>();
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpsc_unbounded_one_to_one_disconnect_receiver_chunk_size_1() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int, 1>();
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpsc_unbounded_many_to_one_disconnect_sender() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int>();
  many_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpsc_unbounded_many_to_one_disconnect_sender_chunk_size_1() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int, 1>();
  many_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpsc_unbounded_many_to_one_disconnect_receiver() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int>();
  many_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpsc_unbounded_many_to_one_disconnect_receiver_chunk_size_1() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int, 1>();
  many_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpsc_unbounded_add_remove_sender() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int, 1>();
  add_remove_sender(std::move(tx), std::move(rx));
}

void mpsc_unbuffered_disconnect_sender() {
  auto [tx, rx] = chan::mpsc::unbuffered::channel<int>();
  unbuffered_disconnect_sender(std::move(tx), std::move(rx));
}

void mpsc_unbuffered_disconnect_receiver() {
  auto [tx, rx] = chan::mpsc::unbuffered::channel<int>();
  disconnect_receiver(std::move(tx), std::move(rx));
}

void mpsc_unbuffered_try() {
  auto [tx, rx] = chan::mpsc::unbuffered::channel<int>();
  unbuffered_try(std::move(tx), std::move(rx));
}

void mpsc_unbuffered_one_to_one_disconnect_sender() {
  auto [tx, rx] = chan::mpsc::unbuffered::channel<int>();
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpsc_unbuffered_one_to_one_disconnect_receiver() {
  auto [tx, rx] = chan::mpsc::unbuffered::channel<int>();
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpsc_unbuffered_many_to_one_disconnect_sender() {
  auto [tx, rx] = chan::mpsc::unbuffered::channel<int>();
  many_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpsc_unbuffered_many_to_one_disconnect_receiver() {
  auto [tx, rx] = chan::mpsc::unbuffered::channel<int>();
  many_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpsc_unbuffered_add_remove_sender() {
  auto [tx, rx] = chan::mpsc::unbuffered::channel<int>();
  add_remove_sender(std::move(tx), std::move(rx));
}

void spmc_bounded_disconnect_sender() {
  auto [tx, rx] = chan::spmc::bounded::channel<int>(16);
  disconnect_sender(std::move(tx), std::move(rx));
}

void spmc_bounded_disconnect_receiver() {
  auto [tx, rx] = chan::spmc::bounded::channel<int>(16);
  disconnect_receiver(std::move(tx), std::move(rx));
}

void spmc_bounded_one_item() {
  auto [tx, rx] = chan::spmc::bounded::channel<int>(16);
  one_item(std::move(tx), std::move(rx));
}

void spmc_bounded_two_seperate() {
  auto [tx, rx] = chan::spmc::bounded::channel<int>(16);
  two_seperate(std::move(tx), std::move(rx));
}

void spmc_bounded_two_consecutive() {
  auto [tx, rx] = chan::spmc::bounded::channel<int>(16);
  two_consecutive(std::move(tx), std::move(rx));
}

void spmc_bounded_leftover() {
  auto [tx, rx] = chan::spmc::bounded::channel<std::unique_ptr<int>>(16);
  leftover(std::move(tx), std::move(rx));
}

void spmc_bounded_try() {
  auto [tx, rx] = chan::spmc::bounded::channel<int>(16);
  bounded_try(std::move(tx), std::move(rx), 16);
}

void spmc_bounded_one_to_one_disconnect_sender() {
  auto [tx, rx] = chan::spmc::bounded::channel<int>(16);
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void spmc_bounded_one_to_one_disconnect_sender_buffer_size_1() {
  auto [tx, rx] = chan::spmc::bounded::channel<int>(1);
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void spmc_bounded_one_to_one_disconnect_receiver() {
  auto [tx, rx] = chan::spmc::bounded::channel<int>(16);
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void spmc_bounded_one_to_one_disconnect_receiver_buffer_size_1() {
  auto [tx, rx] = chan::spmc::bounded::channel<int>(1);
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void spmc_bounded_one_to_many_disconnect_sender() {
  auto [tx, rx] = chan::spmc::bounded::channel<int>(16);
  one_to_many_disconnect_sender(std::move(tx), std::move(rx));
}

void spmc_bounded_one_to_many_disconnect_sender_buffer_size_1() {
  auto [tx, rx] = chan::spmc::bounded::channel<int>(1);
  one_to_many_disconnect_sender(std::move(tx), std::move(rx));
}

void spmc_bounded_one_to_many_disconnect_receiver() {
  auto [tx, rx] = chan::spmc::bounded::channel<int>(16);
  one_to_many_disconnect_receiver(std::move(tx), std::move(rx));
}

void spmc_bounded_one_to_many_disconnect_receiver_buffer_size_1() {
  auto [tx, rx] = chan::spmc::bounded::channel<int>(1);
  one_to_many_disconnect_receiver(std::move(tx), std::move(rx));
}

void spmc_bounded_add_remove_receiver() {
  auto [tx, rx] = chan::spmc::bounded::channel<int>(1);
  add_remove_receiver(std::move(tx), std::move(rx));
}

void spmc_unbounded_disconnect_sender() {
  auto [tx, rx] = chan::spmc::unbounded::channel<int>();
  disconnect_sender(std::move(tx), std::move(rx));
}

void spmc_unbounded_disconnect_receiver() {
  auto [tx, rx] = chan::spmc::unbounded::channel<int>();
  unbounded_disconnect_receiver(std::move(tx), std::move(rx));
}

void spmc_unbounded_one_item() {
  auto [tx, rx] = chan::spmc::unbounded::channel<int>();
  one_item(std::move(tx), std::move(rx));
}

void spmc_unbounded_two_seperate() {
  auto [tx, rx] = chan::spmc::unbounded::channel<int>();
  two_seperate(std::move(tx), std::move(rx));
}

void spmc_unbounded_two_consecutive() {
  auto [tx, rx] = chan::spmc::unbounded::channel<int>();
  two_consecutive(std::move(tx), std::move(rx));
}

void spmc_unbounded_leftover() {
  auto [tx, rx] = chan::spmc::unbounded::channel<std::unique_ptr<int>, 2>();
  leftover(std::move(tx), std::move(rx));
}

void spmc_unbounded_try() {
  auto [tx, rx] = chan::spmc::unbounded::channel<int>();
  unbounded_try(std::move(tx), std::move(rx));
}

void spmc_unbounded_one_to_one_disconnect_sender() {
  auto [tx, rx] = chan::spmc::unbounded::channel<int>();
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void spmc_unbounded_one_to_one_disconnect_sender_chunk_size_1() {
  auto [tx, rx] = chan::spmc::unbounded::channel<int, 1>();
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void spmc_unbounded_one_to_one_disconnect_receiver() {
  auto [tx, rx] = chan::spmc::unbounded::channel<int>();
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void spmc_unbounded_one_to_one_disconnect_receiver_chunk_size_1() {
  auto [tx, rx] = chan::spmc::unbounded::channel<int, 1>();
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void spmc_unbounded_one_to_many_disconnect_sender() {
  auto [tx, rx] = chan::spmc::unbounded::channel<int>();
  one_to_many_disconnect_sender(std::move(tx), std::move(rx));
}

void spmc_unbounded_one_to_many_disconnect_sender_chunk_size_1() {
  auto [tx, rx] = chan::spmc::unbounded::channel<int, 1>();
  one_to_many_disconnect_sender(std::move(tx), std::move(rx));
}

void spmc_unbounded_one_to_many_disconnect_receiver() {
  auto [tx, rx] = chan::spmc::unbounded::channel<int>();
  one_to_many_disconnect_receiver(std::move(tx), std::move(rx));
}

void spmc_unbounded_one_to_many_disconnect_receiver_chunk_size_1() {
  auto [tx, rx] = chan::spmc::unbounded::channel<int, 1>();
  one_to_many_disconnect_receiver(std::move(tx), std::move(rx));
}

void spmc_unbounded_add_remove_receiver() {
  auto [tx, rx] = chan::spmc::unbounded::channel<int, 1>();
  add_remove_receiver(std::move(tx), std::move(rx));
}

void spmc_unbuffered_disconnect_sender() {
  auto [tx, rx] = chan::spmc::unbuffered::channel<int>();
  unbuffered_disconnect_sender(std::move(tx), std::move(rx));
}

void spmc_unbuffered_disconnect_receiver() {
  auto [tx, rx] = chan::spmc::unbuffered::channel<int>();
  disconnect_receiver(std::move(tx), std::move(rx));
}

void spmc_unbuffered_try() {
  auto [tx, rx] = chan::spmc::unbuffered::channel<int>();
  unbuffered_try(std::move(tx), std::move(rx));
}

void spmc_unbuffered_one_to_one_disconnect_sender() {
  auto [tx, rx] = chan::spmc::unbuffered::channel<int>();
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void spmc_unbuffered_one_to_one_disconnect_receiver() {
  auto [tx, rx] = chan::spmc::unbuffered::channel<int>();
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void spmc_unbuffered_one_to_many_disconnect_sender() {
  auto [tx, rx] = chan::spmc::unbuffered::channel<int>();
  one_to_many_disconnect_sender(std::move(tx), std::move(rx));
}

void spmc_unbuffered_one_to_many_disconnect_receiver() {
  auto [tx, rx] = chan::spmc::unbuffered::channel<int>();
  one_to_many_disconnect_receiver(std::move(tx), std::move(rx));
}

void spmc_unbuffered_add_remove_receiver() {
  auto [tx, rx] = chan::spmc::unbuffered::channel<int>();
  add_remove_receiver(std::move(tx), std::move(rx));
}

void mpmc_bounded_disconnect_sender() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(16);
  disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_bounded_disconnect_receiver() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(16);
  disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_bounded_one_item() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(16);
  one_item(std::move(tx), std::move(rx));
}

void mpmc_bounded_two_seperate() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(16);
  two_seperate(std::move(tx), std::move(rx));
}

void mpmc_bounded_two_consecutive() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(16);
  two_consecutive(std::move(tx), std::move(rx));
}

void mpmc_bounded_leftover() {
  auto [tx, rx] = chan::mpmc::bounded::channel<std::unique_ptr<int>>(16);
  leftover(std::move(tx), std::move(rx));
}

void mpmc_bounded_try() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(16);
  bounded_try(std::move(tx), std::move(rx), 16);
}

void mpmc_bounded_one_to_one_disconnect_sender() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(16);
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_bounded_one_to_one_disconnect_sender_buffer_size_1() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(1);
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_bounded_one_to_one_disconnect_receiver() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(16);
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_bounded_one_to_one_disconnect_receiver_buffer_size_1() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(1);
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_bounded_many_to_one_disconnect_sender() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(16);
  many_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_bounded_many_to_one_disconnect_sender_buffer_size_1() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(1);
  many_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_bounded_many_to_one_disconnect_receiver() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(16);
  many_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_bounded_many_to_one_disconnect_receiver_buffer_size_1() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(1);
  many_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_bounded_one_to_many_disconnect_sender() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(16);
  one_to_many_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_bounded_one_to_many_disconnect_sender_buffer_size_1() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(1);
  one_to_many_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_bounded_one_to_many_disconnect_receiver() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(16);
  one_to_many_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_bounded_one_to_many_disconnect_receiver_buffer_size_1() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(1);
  one_to_many_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_bounded_many_to_many_disconnect_sender() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(16);
  many_to_many_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_bounded_many_to_many_disconnect_sender_buffer_size_1() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(1);
  many_to_many_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_bounded_many_to_many_disconnect_receiver() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(16);
  many_to_many_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_bounded_many_to_many_disconnect_receiver_buffer_size_1() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(1);
  many_to_many_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_bounded_add_remove_sender() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(1);
  add_remove_sender(std::move(tx), std::move(rx));
}

void mpmc_bounded_add_remove_receiver() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(1);
  add_remove_receiver(std::move(tx), std::move(rx));
}

void mpmc_bounded_add_remove_sender_receiver() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(1);
  add_remove_sender_receiver(std::move(tx), std::move(rx));
}

void mpmc_unbounded_disconnect_sender() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int>();
  disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_unbounded_disconnect_receiver() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int>();
  unbounded_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_unbounded_one_item() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int>();
  one_item(std::move(tx), std::move(rx));
}

void mpmc_unbounded_two_seperate() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int>();
  two_seperate(std::move(tx), std::move(rx));
}

void mpmc_unbounded_two_consecutive() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int>();
  two_consecutive(std::move(tx), std::move(rx));
}

void mpmc_unbounded_leftover() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<std::unique_ptr<int>, 2>();
  leftover(std::move(tx), std::move(rx));
}

void mpmc_unbounded_try() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int>();
  unbounded_try(std::move(tx), std::move(rx));
}

void mpmc_unbounded_one_to_one_disconnect_sender() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int>();
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_unbounded_one_to_one_disconnect_sender_chunk_size_1() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int, 1>();
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_unbounded_one_to_one_disconnect_receiver() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int>();
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_unbounded_one_to_one_disconnect_receiver_chunk_size_1() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int, 1>();
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_unbounded_many_to_one_disconnect_sender() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int>();
  many_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_unbounded_many_to_one_disconnect_sender_chunk_size_1() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int, 1>();
  many_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_unbounded_many_to_one_disconnect_receiver() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int>();
  many_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_unbounded_many_to_one_disconnect_receiver_chunk_size_1() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int, 1>();
  many_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_unbounded_one_to_many_disconnect_sender() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int>();
  one_to_many_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_unbounded_one_to_many_disconnect_sender_chunk_size_1() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int, 1>();
  one_to_many_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_unbounded_one_to_many_disconnect_receiver() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int>();
  one_to_many_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_unbounded_one_to_many_disconnect_receiver_chunk_size_1() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int, 1>();
  one_to_many_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_unbounded_many_to_many_disconnect_sender() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int>();
  many_to_many_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_unbounded_many_to_many_disconnect_sender_chunk_size_1() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int, 1>();
  many_to_many_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_unbounded_many_to_many_disconnect_receiver() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int>();
  many_to_many_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_unbounded_many_to_many_disconnect_receiver_chunk_size_1() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int, 1>();
  many_to_many_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_unbounded_add_remove_sender() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int, 1>();
  add_remove_sender(std::move(tx), std::move(rx));
}

void mpmc_unbounded_add_remove_receiver() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int, 1>();
  add_remove_receiver(std::move(tx), std::move(rx));
}

void mpmc_unbounded_add_remove_sender_receiver() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int, 1>();
  add_remove_sender_receiver(std::move(tx), std::move(rx));
}

void mpmc_unbuffered_disconnect_sender() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  unbuffered_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_unbuffered_disconnect_receiver() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_unbuffered_try() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  unbuffered_try(std::move(tx), std::move(rx));
}

void mpmc_unbuffered_one_to_one_disconnect_sender() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_unbuffered_one_to_one_disconnect_receiver() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  one_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_unbuffered_many_to_one_disconnect_sender() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  many_to_one_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_unbuffered_many_to_one_disconnect_receiver() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  many_to_one_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_unbuffered_one_to_many_disconnect_sender() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  one_to_many_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_unbuffered_one_to_many_disconnect_receiver() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  one_to_many_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_unbuffered_many_to_many_disconnect_sender() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  many_to_many_disconnect_sender(std::move(tx), std::move(rx));
}

void mpmc_unbuffered_many_to_many_disconnect_receiver() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  many_to_many_disconnect_receiver(std::move(tx), std::move(rx));
}

void mpmc_unbuffered_add_remove_sender() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  add_remove_sender(std::move(tx), std::move(rx));
}

void mpmc_unbuffered_add_remove_receiver() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  add_remove_receiver(std::move(tx), std::move(rx));
}

void mpmc_unbuffered_add_remove_sender_receiver() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  add_remove_sender_receiver(std::move(tx), std::move(rx));
}

struct Test {
  std::string_view name;
  void (*func)();
};

// clang-format off
const Test TESTS[] = {
    Test{"spsc_bounded_disconnect_sender", spsc_bounded_disconnect_sender},
    Test{"spsc_bounded_disconnect_receiver", spsc_bounded_disconnect_receiver},
    Test{"spsc_bounded_one_item", spsc_bounded_one_item},
    Test{"spsc_bounded_two_seperate", spsc_bounded_two_seperate},
    Test{"spsc_bounded_two_consecutive", spsc_bounded_two_consecutive},
    Test{"spsc_bounded_leftover", spsc_bounded_leftover},
    Test{"spsc_bounded_try", spsc_bounded_try},
    Test{"spsc_bounded_one_to_one_disconnect_sender", spsc_bounded_one_to_one_disconnect_sender},
    Test{"spsc_bounded_one_to_one_disconnect_sender_buffer_size_1", spsc_bounded_one_to_one_disconnect_sender_buffer_size_1},
    Test{"spsc_bounded_one_to_one_disconnect_receiver", spsc_bounded_one_to_one_disconnect_receiver},
    Test{"spsc_bounded_one_to_one_disconnect_receiver_buffer_size_1", spsc_bounded_one_to_one_disconnect_receiver_buffer_size_1},
    Test{"spsc_unbounded_disconnect_sender", spsc_unbounded_disconnect_sender},
    Test{"spsc_unbounded_disconnect_receiver", spsc_unbounded_disconnect_receiver},
    Test{"spsc_unbounded_one_item", spsc_unbounded_one_item},
    Test{"spsc_unbounded_two_seperate", spsc_unbounded_two_seperate},
    Test{"spsc_unbounded_two_consecutive", spsc_unbounded_two_consecutive},
    Test{"spsc_unbounded_leftover", spsc_unbounded_leftover},
    Test{"spsc_unbounded_try", spsc_unbounded_try},
    Test{"spsc_unbounded_one_to_one_disconnect_sender", spsc_unbounded_one_to_one_disconnect_sender},
    Test{"spsc_unbounded_one_to_one_disconnect_sender_chunk_size_1", spsc_unbounded_one_to_one_disconnect_sender_chunk_size_1},
    Test{"spsc_unbounded_one_to_one_disconnect_receiver", spsc_unbounded_one_to_one_disconnect_receiver},
    Test{"spsc_unbounded_one_to_one_disconnect_receiver_chunk_size_1", spsc_unbounded_one_to_one_disconnect_receiver_chunk_size_1},
    Test{"spsc_unbuffered_disconnect_sender", spsc_unbuffered_disconnect_sender},
    Test{"spsc_unbuffered_disconnect_receiver", spsc_unbuffered_disconnect_receiver},
    Test{"spsc_unbuffered_try", spsc_unbuffered_try},
    Test{"spsc_unbuffered_one_to_one_disconnect_sender", spsc_unbuffered_one_to_one_disconnect_sender},
    Test{"spsc_unbuffered_one_to_one_disconnect_receiver", spsc_unbuffered_one_to_one_disconnect_receiver},
    Test{"mpsc_bounded_disconnect_sender", mpsc_bounded_disconnect_sender},
    Test{"mpsc_bounded_disconnect_receiver", mpsc_bounded_disconnect_receiver},
    Test{"mpsc_bounded_one_item", mpsc_bounded_one_item},
    Test{"mpsc_bounded_two_seperate", mpsc_bounded_two_seperate},
    Test{"mpsc_bounded_two_consecutive", mpsc_bounded_two_consecutive},
    Test{"mpsc_bounded_leftover", mpsc_bounded_leftover},
    Test{"mpsc_bounded_try", mpsc_bounded_try},
    Test{"mpsc_bounded_one_to_one_disconnect_sender", mpsc_bounded_one_to_one_disconnect_sender},
    Test{"mpsc_bounded_one_to_one_disconnect_sender_buffer_size_1", mpsc_bounded_one_to_one_disconnect_sender_buffer_size_1},
    Test{"mpsc_bounded_one_to_one_disconnect_receiver", mpsc_bounded_one_to_one_disconnect_receiver},
    Test{"mpsc_bounded_one_to_one_disconnect_receiver_buffer_size_1", mpsc_bounded_one_to_one_disconnect_receiver_buffer_size_1},
    Test{"mpsc_bounded_many_to_one_disconnect_sender", mpsc_bounded_many_to_one_disconnect_sender},
    Test{"mpsc_bounded_many_to_one_disconnect_sender_buffer_size_1", mpsc_bounded_many_to_one_disconnect_sender_buffer_size_1},
    Test{"mpsc_bounded_many_to_one_disconnect_receiver", mpsc_bounded_many_to_one_disconnect_receiver},
    Test{"mpsc_bounded_many_to_one_disconnect_receiver_buffer_size_1", mpsc_bounded_many_to_one_disconnect_receiver_buffer_size_1},
    Test{"mpsc_bounded_add_remove_sender", mpsc_bounded_add_remove_sender},
    Test{"mpsc_unbounded_disconnect_sender", mpsc_unbounded_disconnect_sender},
    Test{"mpsc_unbounded_disconnect_receiver", mpsc_unbounded_disconnect_receiver},
    Test{"mpsc_unbounded_one_item", mpsc_unbounded_one_item},
    Test{"mpsc_unbounded_two_seperate", mpsc_unbounded_two_seperate},
    Test{"mpsc_unbounded_two_consecutive", mpsc_unbounded_two_consecutive},
    Test{"mpsc_unbounded_leftover", mpsc_unbounded_leftover},
    Test{"mpsc_unbounded_try", mpsc_unbounded_try},
    Test{"mpsc_unbounded_one_to_one_disconnect_sender", mpsc_unbounded_one_to_one_disconnect_sender},
    Test{"mpsc_unbounded_one_to_one_disconnect_sender_chunk_size_1", mpsc_unbounded_one_to_one_disconnect_sender_chunk_size_1},
    Test{"mpsc_unbounded_one_to_one_disconnect_receiver", mpsc_unbounded_one_to_one_disconnect_receiver},
    Test{"mpsc_unbounded_one_to_one_disconnect_receiver_chunk_size_1", mpsc_unbounded_one_to_one_disconnect_receiver_chunk_size_1},
    Test{"mpsc_unbounded_many_to_one_disconnect_sender", mpsc_unbounded_many_to_one_disconnect_sender},
    Test{"mpsc_unbounded_many_to_one_disconnect_sender_chunk_size_1", mpsc_unbounded_many_to_one_disconnect_sender_chunk_size_1},
    Test{"mpsc_unbounded_many_to_one_disconnect_receiver", mpsc_unbounded_many_to_one_disconnect_receiver},
    Test{"mpsc_unbounded_many_to_one_disconnect_receiver_chunk_size_1", mpsc_unbounded_many_to_one_disconnect_receiver_chunk_size_1},
    Test{"mpsc_unbounded_add_remove_sender", mpsc_unbounded_add_remove_sender},
    Test{"mpsc_unbuffered_disconnect_sender", mpsc_unbuffered_disconnect_sender},
    Test{"mpsc_unbuffered_disconnect_receiver", mpsc_unbuffered_disconnect_receiver},
    Test{"mpsc_unbuffered_try", mpsc_unbuffered_try},
    Test{"mpsc_unbuffered_one_to_one_disconnect_sender", mpsc_unbuffered_one_to_one_disconnect_sender},
    Test{"mpsc_unbuffered_one_to_one_disconnect_receiver", mpsc_unbuffered_one_to_one_disconnect_receiver},
    Test{"mpsc_unbuffered_many_to_one_disconnect_sender", mpsc_unbuffered_many_to_one_disconnect_sender},
    Test{"mpsc_unbuffered_many_to_one_disconnect_receiver", mpsc_unbuffered_many_to_one_disconnect_receiver},
    Test{"mpsc_unbuffered_add_remove_sender", mpsc_unbuffered_add_remove_sender},
    Test{"spmc_bounded_disconnect_sender", spmc_bounded_disconnect_sender},
    Test{"spmc_bounded_disconnect_receiver", spmc_bounded_disconnect_receiver},
    Test{"spmc_bounded_one_item", spmc_bounded_one_item},
    Test{"spmc_bounded_two_seperate", spmc_bounded_two_seperate},
    Test{"spmc_bounded_two_consecutive", spmc_bounded_two_consecutive},
    Test{"spmc_bounded_leftover", spmc_bounded_leftover},
    Test{"spmc_bounded_try", spmc_bounded_try},
    Test{"spmc_bounded_one_to_one_disconnect_sender", spmc_bounded_one_to_one_disconnect_sender},
    Test{"spmc_bounded_one_to_one_disconnect_sender_buffer_size_1", spmc_bounded_one_to_one_disconnect_sender_buffer_size_1},
    Test{"spmc_bounded_one_to_one_disconnect_receiver", spmc_bounded_one_to_one_disconnect_receiver},
    Test{"spmc_bounded_one_to_one_disconnect_receiver_buffer_size_1", spmc_bounded_one_to_one_disconnect_receiver_buffer_size_1},
    Test{"spmc_bounded_one_to_many_disconnect_sender", spmc_bounded_one_to_many_disconnect_sender},
    Test{"spmc_bounded_one_to_many_disconnect_sender_buffer_size_1", spmc_bounded_one_to_many_disconnect_sender_buffer_size_1},
    Test{"spmc_bounded_one_to_many_disconnect_receiver", spmc_bounded_one_to_many_disconnect_receiver},
    Test{"spmc_bounded_one_to_many_disconnect_receiver_buffer_size_1", spmc_bounded_one_to_many_disconnect_receiver_buffer_size_1},
    Test{"spmc_bounded_add_remove_receiver", spmc_bounded_add_remove_receiver},
    Test{"spmc_unbounded_disconnect_sender", spmc_unbounded_disconnect_sender},
    Test{"spmc_unbounded_disconnect_receiver", spmc_unbounded_disconnect_receiver},
    Test{"spmc_unbounded_one_item", spmc_unbounded_one_item},
    Test{"spmc_unbounded_two_seperate", spmc_unbounded_two_seperate},
    Test{"spmc_unbounded_two_consecutive", spmc_unbounded_two_consecutive},
    Test{"spmc_unbounded_try", spmc_unbounded_try},
    Test{"spmc_unbounded_one_to_one_disconnect_sender", spmc_unbounded_one_to_one_disconnect_sender},
    Test{"spmc_unbounded_one_to_one_disconnect_sender_chunk_size_1", spmc_unbounded_one_to_one_disconnect_sender_chunk_size_1},
    Test{"spmc_unbounded_one_to_one_disconnect_receiver", spmc_unbounded_one_to_one_disconnect_receiver},
    Test{"spmc_unbounded_one_to_one_disconnect_receiver_chunk_size_1", spmc_unbounded_one_to_one_disconnect_receiver_chunk_size_1},
    Test{"spmc_unbounded_one_to_many_disconnect_sender", spmc_unbounded_one_to_many_disconnect_sender},
    Test{"spmc_unbounded_one_to_many_disconnect_sender_chunk_size_1", spmc_unbounded_one_to_many_disconnect_sender_chunk_size_1},
    Test{"spmc_unbounded_one_to_many_disconnect_receiver", spmc_unbounded_one_to_many_disconnect_receiver},
    Test{"spmc_unbounded_one_to_many_disconnect_receiver_chunk_size_1", spmc_unbounded_one_to_many_disconnect_receiver_chunk_size_1},
    Test{"spmc_unbounded_add_remove_receiver", spmc_unbounded_add_remove_receiver},
    Test{"spmc_unbuffered_disconnect_sender", spmc_unbuffered_disconnect_sender},
    Test{"spmc_unbuffered_disconnect_receiver", spmc_unbuffered_disconnect_receiver},
    Test{"spmc_unbuffered_try", spmc_unbuffered_try},
    Test{"spmc_unbuffered_one_to_one_disconnect_sender", spmc_unbuffered_one_to_one_disconnect_sender},
    Test{"spmc_unbuffered_one_to_one_disconnect_receiver", spmc_unbuffered_one_to_one_disconnect_receiver},
    Test{"spmc_unbuffered_one_to_many_disconnect_sender", spmc_unbuffered_one_to_many_disconnect_sender},
    Test{"spmc_unbuffered_one_to_many_disconnect_receiver", spmc_unbuffered_one_to_many_disconnect_receiver},
    Test{"spmc_unbuffered_add_remove_receiver", spmc_unbuffered_add_remove_receiver},
    Test{"mpmc_bounded_disconnect_sender", mpmc_bounded_disconnect_sender},
    Test{"mpmc_bounded_disconnect_receiver", mpmc_bounded_disconnect_receiver},
    Test{"mpmc_bounded_one_item", mpmc_bounded_one_item},
    Test{"mpmc_bounded_two_seperate", mpmc_bounded_two_seperate},
    Test{"mpmc_bounded_two_consecutive", mpmc_bounded_two_consecutive},
    Test{"mpmc_bounded_leftover", mpmc_bounded_leftover},
    Test{"mpmc_bounded_try", mpmc_bounded_try},
    Test{"mpmc_bounded_one_to_one_disconnect_sender", mpmc_bounded_one_to_one_disconnect_sender},
    Test{"mpmc_bounded_one_to_one_disconnect_sender_buffer_size_1", mpmc_bounded_one_to_one_disconnect_sender_buffer_size_1},
    Test{"mpmc_bounded_one_to_one_disconnect_receiver", mpmc_bounded_one_to_one_disconnect_receiver},
    Test{"mpmc_bounded_one_to_one_disconnect_receiver_buffer_size_1", mpmc_bounded_one_to_one_disconnect_receiver_buffer_size_1},
    Test{"mpmc_bounded_many_to_one_disconnect_sender", mpmc_bounded_many_to_one_disconnect_sender},
    Test{"mpmc_bounded_many_to_one_disconnect_sender_buffer_size_1", mpmc_bounded_many_to_one_disconnect_sender_buffer_size_1},
    Test{"mpmc_bounded_many_to_one_disconnect_receiver", mpmc_bounded_many_to_one_disconnect_receiver},
    Test{"mpmc_bounded_many_to_one_disconnect_receiver_buffer_size_1", mpmc_bounded_many_to_one_disconnect_receiver_buffer_size_1},
    Test{"mpmc_bounded_one_to_many_disconnect_sender", mpmc_bounded_one_to_many_disconnect_sender},
    Test{"mpmc_bounded_one_to_many_disconnect_sender_buffer_size_1", mpmc_bounded_one_to_many_disconnect_sender_buffer_size_1},
    Test{"mpmc_bounded_one_to_many_disconnect_receiver", mpmc_bounded_one_to_many_disconnect_receiver},
    Test{"mpmc_bounded_one_to_many_disconnect_receiver_buffer_size_1", mpmc_bounded_one_to_many_disconnect_receiver_buffer_size_1},
    Test{"mpmc_bounded_many_to_many_disconnect_sender", mpmc_bounded_many_to_many_disconnect_sender},
    Test{"mpmc_bounded_many_to_many_disconnect_sender_buffer_size_1", mpmc_bounded_many_to_many_disconnect_sender_buffer_size_1},
    Test{"mpmc_bounded_many_to_many_disconnect_receiver", mpmc_bounded_many_to_many_disconnect_receiver},
    Test{"mpmc_bounded_many_to_many_disconnect_receiver_buffer_size_1", mpmc_bounded_many_to_many_disconnect_receiver_buffer_size_1},
    Test{"mpmc_bounded_add_remove_sender", mpmc_bounded_add_remove_sender},
    Test{"mpmc_bounded_add_remove_receiver", mpmc_bounded_add_remove_receiver},
    Test{"mpmc_bounded_add_remove_sender_receiver", mpmc_bounded_add_remove_sender_receiver},
    Test{"mpmc_unbounded_disconnect_sender", mpmc_unbounded_disconnect_sender},
    Test{"mpmc_unbounded_disconnect_receiver", mpmc_unbounded_disconnect_receiver},
    Test{"mpmc_unbounded_one_item", mpmc_unbounded_one_item},
    Test{"mpmc_unbounded_two_seperate", mpmc_unbounded_two_seperate},
    Test{"mpmc_unbounded_two_consecutive", mpmc_unbounded_two_consecutive},
    Test{"mpmc_unbounded_try", mpmc_unbounded_try},
    Test{"mpmc_unbounded_one_to_one_disconnect_sender", mpmc_unbounded_one_to_one_disconnect_sender},
    Test{"mpmc_unbounded_one_to_one_disconnect_sender_chunk_size_1", mpmc_unbounded_one_to_one_disconnect_sender_chunk_size_1},
    Test{"mpmc_unbounded_one_to_one_disconnect_receiver", mpmc_unbounded_one_to_one_disconnect_receiver},
    Test{"mpmc_unbounded_one_to_one_disconnect_receiver_chunk_size_1", mpmc_unbounded_one_to_one_disconnect_receiver_chunk_size_1},
    Test{"mpmc_unbounded_many_to_one_disconnect_sender", mpmc_unbounded_many_to_one_disconnect_sender},
    Test{"mpmc_unbounded_many_to_one_disconnect_sender_chunk_size_1", mpmc_unbounded_many_to_one_disconnect_sender_chunk_size_1},
    Test{"mpmc_unbounded_many_to_one_disconnect_receiver", mpmc_unbounded_many_to_one_disconnect_receiver},
    Test{"mpmc_unbounded_many_to_one_disconnect_receiver_chunk_size_1", mpmc_unbounded_many_to_one_disconnect_receiver_chunk_size_1},
    Test{"mpmc_unbounded_one_to_many_disconnect_sender", mpmc_unbounded_one_to_many_disconnect_sender},
    Test{"mpmc_unbounded_one_to_many_disconnect_sender_chunk_size_1", mpmc_unbounded_one_to_many_disconnect_sender_chunk_size_1},
    Test{"mpmc_unbounded_one_to_many_disconnect_receiver", mpmc_unbounded_one_to_many_disconnect_receiver},
    Test{"mpmc_unbounded_one_to_many_disconnect_receiver_chunk_size_1", mpmc_unbounded_one_to_many_disconnect_receiver_chunk_size_1},
    Test{"mpmc_unbounded_many_to_many_disconnect_sender", mpmc_unbounded_many_to_many_disconnect_sender},
    Test{"mpmc_unbounded_many_to_many_disconnect_sender_chunk_size_1", mpmc_unbounded_many_to_many_disconnect_sender_chunk_size_1},
    Test{"mpmc_unbounded_many_to_many_disconnect_receiver", mpmc_unbounded_many_to_many_disconnect_receiver},
    Test{"mpmc_unbounded_many_to_many_disconnect_receiver_chunk_size_1", mpmc_unbounded_many_to_many_disconnect_receiver_chunk_size_1},
    Test{"mpmc_unbounded_add_remove_sender", mpmc_unbounded_add_remove_sender},
    Test{"mpmc_unbounded_add_remove_receiver", mpmc_unbounded_add_remove_receiver},
    Test{"mpmc_unbounded_add_remove_sender_receiver", mpmc_unbounded_add_remove_sender_receiver},
    Test{"mpmc_unbuffered_disconnect_sender", mpmc_unbuffered_disconnect_sender},
    Test{"mpmc_unbuffered_disconnect_receiver", mpmc_unbuffered_disconnect_receiver},
    Test{"mpmc_unbuffered_try", mpmc_unbuffered_try},
    Test{"mpmc_unbuffered_one_to_one_disconnect_sender", mpmc_unbuffered_one_to_one_disconnect_sender},
    Test{"mpmc_unbuffered_one_to_one_disconnect_receiver", mpmc_unbuffered_one_to_one_disconnect_receiver},
    Test{"mpmc_unbuffered_many_to_one_disconnect_sender", mpmc_unbuffered_many_to_one_disconnect_sender},
    Test{"mpmc_unbuffered_many_to_one_disconnect_receiver", mpmc_unbuffered_many_to_one_disconnect_receiver},
    Test{"mpmc_unbuffered_one_to_many_disconnect_sender", mpmc_unbuffered_one_to_many_disconnect_sender},
    Test{"mpmc_unbuffered_one_to_many_disconnect_receiver", mpmc_unbuffered_one_to_many_disconnect_receiver},
    Test{"mpmc_unbuffered_many_to_many_disconnect_sender", mpmc_unbuffered_many_to_many_disconnect_sender},
    Test{"mpmc_unbuffered_many_to_many_disconnect_receiver", mpmc_unbuffered_many_to_many_disconnect_receiver},
    Test{"mpmc_unbuffered_add_remove_sender", mpmc_unbuffered_add_remove_sender},
    Test{"mpmc_unbuffered_add_remove_receiver", mpmc_unbuffered_add_remove_receiver},
    Test{"mpmc_unbuffered_add_remove_sender_receiver", mpmc_unbuffered_add_remove_sender_receiver},
};
// clang-format on

int main(int argc, char **argv) {
  auto fail_count = 0;
  for (const auto &[name, func] : TESTS) {
    bool should_run;
    if (argc > 1) {
      if (auto arg = std::string_view(argv[1]);
          !arg.empty() && arg.front() == '-') {
        should_run = true;
      } else {
        should_run = false;
      }
      for (int i = 1; i < argc; ++i) {
        auto arg = std::string_view(argv[i]);
        if (!arg.empty() && arg.front() == '-') {
          arg.remove_prefix(1);
          if (name.contains(arg)) {
            should_run = false;
          }
        } else {
          if (name.contains(arg)) {
            should_run = true;
          }
        }
      }
    } else {
      should_run = true;
    }

    if (should_run) {
      std::cout << "Running test " << name << '\n';
      try {
        func();
        std::cout << "  PASS\n";
      } catch (const std::exception &e) {
        std::cout << "  FAIL: " << e.what() << '\n';
        ++fail_count;
      }
    }
  }
  if (fail_count == 0) {
    std::cout << "All tests passed.\n";
    return 0;
  } else {
    std::cout << "Failed " << fail_count << " tests.\n";
    return 1;
  }
}
