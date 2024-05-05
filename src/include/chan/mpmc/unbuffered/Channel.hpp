#pragma once

#include <atomic>
#include <expected>
#include <optional>
#include <semaphore>

#include "../../RecvError.hpp"
#include "../../detail/UnbufferedChannel.hpp"

namespace chan::mpmc::unbuffered {
template <typename T> class Channel : detail::UnbufferedChannel<Channel<T>, T> {
  friend class detail::UnbufferedChannel<Channel, T>;
  template <typename, typename> friend class Sender;
  template <typename, typename> friend class Receiver;

  std::atomic<std::optional<T> *> packet;
  std::counting_semaphore<> send_ready;
  std::counting_semaphore<> recv_ready;
  std::mutex recv_mutex;
  std::atomic_size_t sender_count;
  std::atomic_size_t receiver_count;
  std::atomic_bool disconnected;

public:
  Channel()
      : packet(nullptr), send_ready(0), recv_ready(0), sender_count(1),
        receiver_count(1), disconnected(false) {}

private:
  bool send_impl(T &item) {
    auto packet = this->packet.exchange(nullptr, std::memory_order::relaxed);
    if (packet) {
      *packet = std::move(item);
      return true;
    } else {
      return false;
    }
  }

  std::expected<T, RecvError> recv() {
    std::optional<T> packet;
    std::lock_guard recv_lock(this->recv_mutex);
    this->packet.store(&packet, std::memory_order::relaxed);
    this->send_ready.release();
    this->recv_ready.acquire();
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
      this->recv_ready.release(receiver_count * 2);
      return destroy;
    }
    return false;
  }

  bool release_receiver() {
    if (this->receiver_count.fetch_sub(1, std::memory_order::acq_rel) == 1) {
      auto destroy =
          this->disconnected.exchange(true, std::memory_order::relaxed);
      auto sender_count = this->sender_count.load(std::memory_order::relaxed);
      this->send_ready.release(sender_count * 2);
      return destroy;
    }
    return false;
  }
};
} // namespace chan::mpmc::unbuffered
