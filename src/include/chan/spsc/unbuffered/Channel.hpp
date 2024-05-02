#pragma once

#include <atomic>
#include <expected>
#include <optional>
#include <semaphore>

#include "../../RecvError.hpp"
#include "../../SendError.hpp"

namespace chan::spsc::unbuffered {
template <typename T> class Channel {
  template <typename, typename> friend class Sender;
  template <typename, typename> friend class Receiver;

  std::atomic<std::optional<T> *> packet;
  std::counting_semaphore<2> read_ready;
  std::counting_semaphore<2> write_ready;
  std::atomic_bool disconnected;

public:
  Channel()
      : packet(nullptr), read_ready(0), write_ready(0), disconnected(false) {}

private:
  std::expected<void, SendError<T>> send(T item) {
    this->write_ready.acquire();
    if (!this->send_impl(item)) {
      return std::unexpected(SendError(std::move(item)));
    }
    return {};
  }

  template <typename Rep, typename Period>
  std::expected<void, TrySendError<T>>
  try_send_for(T item, const std::chrono::duration<Rep, Period> &timeout) {
    if (!this->write_ready.try_acquire_for(timeout)) {
      return std::unexpected(
          TrySendError(TrySendErrorKind::Full, std::move(item)));
    }
    return this->try_send_impl(std::move(item));
  }

  template <typename Clock, typename Duration>
  std::expected<void, TrySendError<T>>
  try_send_until(T item,
                 const std::chrono::time_point<Clock, Duration> &deadline) {
    if (!this->write_ready.try_acquire_until(deadline)) {
      return std::unexpected(
          TrySendError(TrySendErrorKind::Full, std::move(item)));
    }
    return this->try_send_impl(std::move(item));
  }

  std::expected<void, TrySendError<T>> try_send_impl(T &&item) {
    if (!this->send_impl(item)) {
      return std::unexpected(
          TrySendError(TrySendErrorKind::Full, std::move(item)));
    }
    return {};
  }

  bool send_impl(T &item) {
    auto packet = this->packet.exchange(nullptr, std::memory_order::relaxed);
    if (!packet) {
      return false;
    }
    *packet = std::move(item);
    this->read_ready.release();
    return true;
  }

  std::expected<T, RecvError> recv() {
    std::optional<T> packet;
    this->packet.store(&packet, std::memory_order::relaxed);
    this->write_ready.release();
    this->read_ready.acquire();
    if (!packet) {
      return std::unexpected(RecvError());
    }
    return std::move(*packet);
  }

  bool release_sender() {
    this->read_ready.release();
    return this->disconnected.exchange(true, std::memory_order::relaxed);
  }

  bool release_receiver() {
    this->write_ready.release();
    return this->disconnected.exchange(true, std::memory_order::relaxed);
  }
};
} // namespace chan::spsc::unbuffered
