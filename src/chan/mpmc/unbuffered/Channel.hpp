#ifndef _CHAN_MPMC_UNBUFFERED_CHANNEL_H
#define _CHAN_MPMC_UNBUFFERED_CHANNEL_H

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
    std::size_t sender_count;
    do {
      sender_count = this->sender_count.load(std::memory_order::relaxed);
    } while (sender_count != 0 &&
             !this->sender_count.compare_exchange_weak(
                 sender_count, sender_count + 1, std::memory_order::relaxed));
    return sender_count != 0;
  }

  bool acquire_receiver() {
    std::size_t receiver_count;
    do {
      receiver_count = this->receiver_count.load(std::memory_order::relaxed);
    } while (receiver_count != 0 && !this->receiver_count.compare_exchange_weak(
                                        receiver_count, receiver_count + 1,
                                        std::memory_order::relaxed));
    return receiver_count != 0;
  }

  bool release_sender() {
    if (this->sender_count.fetch_sub(1, std::memory_order::acq_rel) != 1) {
      return false;
    }
    auto receiver_count =
        this->receiver_count.exchange(0, std::memory_order::relaxed);
    this->recv_ready.release(receiver_count);
    return this->disconnected.exchange(true, std::memory_order::relaxed);
  }

  bool release_receiver() {
    if (this->receiver_count.fetch_sub(1, std::memory_order::acq_rel) != 1) {
      return false;
    }
    auto sender_count =
        this->sender_count.exchange(0, std::memory_order::relaxed);
    this->send_ready.release(sender_count);
    return this->disconnected.exchange(true, std::memory_order::relaxed);
  }
};
} // namespace chan::mpmc::unbuffered

#endif
