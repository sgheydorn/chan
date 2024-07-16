#ifndef _CHAN_SPMC_BOUNDED_CHANNEL_H
#define _CHAN_SPMC_BOUNDED_CHANNEL_H

#include <atomic>
#include <memory>
#include <optional>
#include <thread>

#include "../../detail/BoundedChannel.hpp"
#include "../../detail/SemaphoreType.hpp"
#include "../Packet.hpp"

namespace chan::spmc::bounded {
/// Channel implementation.
///
/// Aside from custom allocators, there is no reason to work with this class
/// directly.
template <typename T, typename A>
class Chan : detail::BoundedChannel<Chan<T, A>, T> {
  friend struct detail::BoundedChannel<Chan, T>;
  template <typename, typename, typename> friend class Sender;
  template <typename, typename, typename> friend class Receiver;

  A allocator;
  std::allocator_traits<A>::pointer packet_buffer;
  std::size_t capacity;
  std::atomic_size_t head_index;
  std::size_t tail_index;
  std::atomic_size_t size;
  detail::SemaphoreType send_ready;
  detail::SemaphoreType recv_ready;
  std::atomic_bool _send_done;
  std::atomic_size_t receiver_count;
  std::atomic_bool disconnected;

public:
  /// Create a channel that assumes a single `Sender` and single `Receiver`.
  ///
  /// This constructor should not be called directly. Instead, call the
  /// `channel` function.
  Chan(std::size_t capacity, A allocator)
      : allocator(std::move(allocator)),
        packet_buffer(
            std::allocator_traits<A>::allocate(this->allocator, capacity)),
        capacity(capacity), head_index(0), tail_index(0), size(0),
        send_ready(capacity), recv_ready(0), _send_done(false),
        receiver_count(1), disconnected(false) {
    for (std::size_t index = 0; index < capacity; ++index) {
      std::allocator_traits<A>::construct(
          this->allocator, &this->packet_buffer[index].write_ready, true);
    }
  }

  ~Chan() {
    auto index = this->head_index.load(std::memory_order::relaxed);
    while (index != this->tail_index) {
      std::allocator_traits<A>::destroy(this->allocator,
                                        &this->packet_buffer[index].item);
      if (++index == this->capacity) {
        index = 0;
      }
    }

    for (std::size_t index = 0; index < this->capacity; ++index) {
      std::allocator_traits<A>::destroy(
          this->allocator, &this->packet_buffer[index].write_ready);
    }

    std::allocator_traits<A>::deallocate(this->allocator, this->packet_buffer,
                                         this->capacity);
  }

private:
  void do_send(T item) {
    auto &packet = this->packet_buffer[this->tail_index];
    while (!packet.write_ready.exchange(false, std::memory_order::acquire)) {
      std::this_thread::yield();
    }
    std::allocator_traits<A>::construct(this->allocator, &packet.item,
                                        std::move(item));
    if (++this->tail_index == this->capacity) {
      this->tail_index = 0;
    }
  }

  T do_recv() {
    std::size_t head_index;
    do {
      head_index = this->head_index.load(std::memory_order::relaxed);
    } while (!this->head_index.compare_exchange_weak(
        head_index, head_index == this->capacity - 1 ? 0 : head_index + 1,
        std::memory_order::relaxed));

    auto &packet = this->packet_buffer[head_index];
    auto item = std::move(packet.item);
    std::allocator_traits<A>::destroy(this->allocator, &packet.item);
    packet.write_ready.store(true, std::memory_order::release);
    return item;
  }

  bool send_done() const {
    return this->_send_done.load(std::memory_order::acquire);
  }

  bool recv_done() const {
    return this->receiver_count.load(std::memory_order::acquire) == 0;
  }

  void acquire_receiver() {
    this->receiver_count.fetch_add(1, std::memory_order::relaxed);
  }

  bool release_sender() {
    this->_send_done.store(true, std::memory_order::release);
    auto receiver_count = this->receiver_count.load(std::memory_order::relaxed);
    this->recv_ready.release(receiver_count);
    return this->disconnected.exchange(true, std::memory_order::acq_rel);
  }

  bool release_receiver() {
    if (this->receiver_count.fetch_sub(1, std::memory_order::acq_rel) != 1) {
      return false;
    }
    this->send_ready.release();
    return this->disconnected.exchange(true, std::memory_order::acq_rel);
  }
};
} // namespace chan::spmc::bounded

#endif
