#ifndef _CHAN_MPMC_BOUNDED_CHANNEL_H
#define _CHAN_MPMC_BOUNDED_CHANNEL_H

#include <atomic>
#include <memory>
#include <optional>
#include <thread>

#include "../../detail/BoundedChannel.hpp"
#include "../../detail/SemaphoreType.hpp"
#include "../Packet.hpp"

namespace chan::mpmc::bounded {
template <typename T, typename A>
class Channel : detail::BoundedChannel<Channel<T, A>, T> {
  friend struct detail::BoundedChannel<Channel, T>;
  template <typename, typename, typename> friend class Sender;
  template <typename, typename, typename> friend class Receiver;

  A allocator;
  std::allocator_traits<A>::pointer packet_buffer;
  std::size_t capacity;
  std::atomic_size_t head_index;
  std::atomic_size_t tail_index;
  std::atomic_size_t size;
  detail::SemaphoreType send_ready;
  detail::SemaphoreType recv_ready;
  std::atomic_size_t sender_count;
  std::atomic_size_t receiver_count;
  std::atomic_bool recv_done;
  std::atomic_bool disconnected;

public:
  Channel(std::size_t capacity, A allocator)
      : allocator(std::move(allocator)),
        packet_buffer(
            std::allocator_traits<A>::allocate(this->allocator, capacity)),
        capacity(capacity), head_index(0), tail_index(0), size(0),
        send_ready(capacity), recv_ready(0), sender_count(1), receiver_count(1),
        recv_done(false), disconnected(false) {
    for (std::size_t index = 0; index < capacity; ++index) {
      std::allocator_traits<A>::construct(
          this->allocator, &this->packet_buffer[index].read_ready, false);
      std::allocator_traits<A>::construct(
          this->allocator, &this->packet_buffer[index].write_ready, true);
    }
  }

  ~Channel() {
    auto index = this->head_index.load(std::memory_order::relaxed);
    auto tail_index = this->tail_index.load(std::memory_order::relaxed);
    while (index != tail_index) {
      std::allocator_traits<A>::destroy(this->allocator,
                                        this->packet_buffer + index);
      if (++index == this->capacity) {
        index = 0;
      }
    }
    std::allocator_traits<A>::deallocate(this->allocator, this->packet_buffer,
                                         this->capacity);
  }

private:
  bool send_impl(T &item) {
    if (this->recv_done.load(std::memory_order::relaxed)) {
      return false;
    }

    std::size_t tail_index;
    do {
      tail_index = this->tail_index.load(std::memory_order::relaxed);
    } while (!this->tail_index.compare_exchange_weak(
        tail_index, tail_index == this->capacity - 1 ? 0 : tail_index + 1,
        std::memory_order::relaxed));

    auto &packet = this->packet_buffer[tail_index];
    while (!packet.write_ready.exchange(false, std::memory_order::acquire)) {
      std::this_thread::yield();
    }
    std::allocator_traits<A>::construct(this->allocator, &packet.item,
                                        std::move(item));
    packet.read_ready.store(true, std::memory_order::release);
    this->size.fetch_add(1, std::memory_order::relaxed);
    return true;
  }

  std::optional<T> recv_impl() {
    std::size_t size;
    do {
      size = this->size.load(std::memory_order::relaxed);
    } while (size != 0 && !this->size.compare_exchange_weak(
                              size, size - 1, std::memory_order::relaxed));

    if (size == 0) {
      return {};
    }

    std::size_t head_index;
    do {
      head_index = this->head_index.load(std::memory_order::relaxed);
    } while (!this->head_index.compare_exchange_weak(
        head_index, head_index == this->capacity - 1 ? 0 : head_index + 1,
        std::memory_order::relaxed));

    auto &packet = this->packet_buffer[head_index];
    while (!packet.read_ready.exchange(false, std::memory_order::acquire)) {
      std::this_thread::yield();
    }
    auto item = std::move(packet.item);
    std::allocator_traits<A>::destroy(this->allocator, &packet.item);
    packet.write_ready.store(true, std::memory_order::release);
    return item;
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
    this->recv_done.store(true, std::memory_order::relaxed);
    auto sender_count =
        this->sender_count.exchange(0, std::memory_order::relaxed);
    this->send_ready.release(sender_count);
    return this->disconnected.exchange(true, std::memory_order::relaxed);
  }
};
} // namespace chan::mpmc::bounded

#endif
