#pragma once

#include <atomic>
#include <expected>
#include <optional>
#include <semaphore>

#include "../../RecvError.hpp"
#include "../../SendError.hpp"

namespace chan::mpmc::unbuffered {
template <typename T> class Channel {
  template <typename, typename> friend class Sender;
  template <typename, typename> friend class Receiver;

  std::atomic<std::optional<T> *> packet;
  std::counting_semaphore<> read_ready;
  std::counting_semaphore<> write_ready;
  std::mutex recv_mutex;
  std::atomic_size_t sender_count;
  std::atomic_size_t receiver_count;
  std::atomic_bool disconnected;

public:
  Channel()
      : packet(nullptr), read_ready(0), write_ready(0), sender_count(1),
        receiver_count(1), disconnected(false) {}

private:
  std::expected<void, SendError<T>> send(T item) {
    this->write_ready.acquire();
    auto packet = this->packet.exchange(nullptr, std::memory_order::relaxed);
    if (!packet) {
      return std::unexpected(SendError(std::move(item)));
    }
    *packet = std::move(item);
    this->read_ready.release();
    return {};
  }

  template <typename Rep, typename Period>
  std::expected<void, TrySendError<T>>
  try_send_for(T item, const std::chrono::duration<Rep, Period> &timeout);

  template <typename Clock, typename Duration>
  std::expected<void, TrySendError<T>>
  try_send_until(T item,
                 const std::chrono::time_point<Clock, Duration> &deadline);

  std::expected<T, RecvError> recv() {
    std::optional<T> packet;
    std::lock_guard recv_lock(this->recv_mutex);
    this->packet.store(&packet, std::memory_order::relaxed);
    this->write_ready.release();
    this->read_ready.acquire();
    if (!packet) {
      return std::unexpected(RecvError());
    }
    return std::move(*packet);
  }

  bool acquire_sender() {
    if (this->disconnected.load(std::memory_order::relaxed)) {
      return false;
    }
    this->sender_count.fetch_add(1, std::memory_order::relaxed);
    return true;
  }

  bool acquire_receiver() {
    if (this->disconnected.load(std::memory_order::relaxed)) {
      return false;
    }
    this->receiver_count.fetch_add(1, std::memory_order::relaxed);
    return true;
  }

  bool release_sender() {
    if (this->sender_count.fetch_sub(1, std::memory_order::acq_rel) == 1) {
      auto destroy =
          this->disconnected.exchange(true, std::memory_order::relaxed);
      auto receiver_count =
          this->receiver_count.load(std::memory_order::relaxed);
      this->read_ready.release(receiver_count * 2);
      return destroy;
    }
    return false;
  }

  bool release_receiver() {
    if (this->receiver_count.fetch_sub(1, std::memory_order::acq_rel) == 1) {
      auto destroy =
          this->disconnected.exchange(true, std::memory_order::relaxed);
      auto sender_count = this->sender_count.load(std::memory_order::relaxed);
      this->write_ready.release(sender_count * 2);
      return destroy;
    }
    return false;
  }
};
} // namespace chan::mpmc::unbuffered
