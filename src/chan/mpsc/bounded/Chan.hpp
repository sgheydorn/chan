#ifndef _CHAN_MPSC_BOUNDED_CHANNEL_H
#define _CHAN_MPSC_BOUNDED_CHANNEL_H

#include <atomic>
#include <memory>
#include <optional>
#include <thread>

#include "../../detail/BoundedChannel.hpp"
#include "../../detail/SemaphoreType.hpp"
#include "../Packet.hpp"

namespace chan::mpsc::bounded {
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
  std::size_t head_index;
  std::atomic_size_t tail_index;
  std::atomic_size_t size;
  detail::SemaphoreType send_ready;
  detail::SemaphoreType recv_ready;
  std::atomic_size_t sender_count;
  std::atomic_bool _recv_done;
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
        send_ready(capacity), recv_ready(0), sender_count(1), _recv_done(false),
        disconnected(false) {
    for (std::size_t index = 0; index < capacity; ++index) {
      std::allocator_traits<A>::construct(
          this->allocator, &this->packet_buffer[index].ready, false);
    }
  }

  ~Chan() {
    auto index = this->head_index;
    auto tail_index = this->tail_index.load(std::memory_order::relaxed);
    while (index != tail_index) {
      std::allocator_traits<A>::destroy(this->allocator,
                                        &this->packet_buffer[index].item);
      if (++index == this->capacity) {
        index = 0;
      }
    }

    for (std::size_t index = 0; index < this->capacity; ++index) {
      std::allocator_traits<A>::destroy(this->allocator,
                                        &this->packet_buffer[index].ready);
    }

    std::allocator_traits<A>::deallocate(this->allocator, this->packet_buffer,
                                         this->capacity);
  }

private:
  void do_send(T item) {
    std::size_t tail_index;
    do {
      tail_index = this->tail_index.load(std::memory_order::relaxed);
    } while (!this->tail_index.compare_exchange_weak(
        tail_index, tail_index == this->capacity - 1 ? 0 : tail_index + 1,
        std::memory_order::relaxed));

    auto &packet = this->packet_buffer[tail_index];
    std::allocator_traits<A>::construct(this->allocator, &packet.item,
                                        std::move(item));
    packet.ready.store(true, std::memory_order::release);
  }

  T do_recv() {
    auto &packet = this->packet_buffer[this->head_index];
    while (!packet.ready.exchange(false, std::memory_order::acquire)) {
      std::this_thread::yield();
    }
    auto item = std::move(packet.item);
    std::allocator_traits<A>::destroy(this->allocator, &packet.item);
    if (++this->head_index == this->capacity) {
      this->head_index = 0;
    }
    return item;
  }

  bool send_done() const {
    return this->sender_count.load(std::memory_order::acquire) == 0;
  }

  bool recv_done() const {
    return this->_recv_done.load(std::memory_order::acquire);
  }

  void acquire_sender() {
    this->sender_count.fetch_add(1, std::memory_order::relaxed);
  }

  bool release_sender() {
    if (this->sender_count.fetch_sub(1, std::memory_order::acq_rel) != 1) {
      return false;
    }
    this->recv_ready.release();
    return this->disconnected.exchange(true, std::memory_order::acq_rel);
  }

  bool release_receiver() {
    this->_recv_done.store(true, std::memory_order::release);
    auto sender_count = this->sender_count.load(std::memory_order::relaxed);
    this->send_ready.release(sender_count);
    return this->disconnected.exchange(true, std::memory_order::acq_rel);
  }
};
} // namespace chan::mpsc::bounded

#endif
