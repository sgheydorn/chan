#pragma once

#include <atomic>
#include <expected>
#include <memory>
#include <semaphore>
#include <thread>

#include "../../detail/BoundedChannel.hpp"
#include "../Packet.hpp"

namespace chan::mpmc::bounded {
template <typename T, typename A>
class Channel : detail::BoundedChannel<Channel<T, A>, T> {
  friend class detail::BoundedChannel<Channel, T>;
  template <typename, typename, typename> friend class Sender;
  template <typename, typename, typename> friend class Receiver;

  A allocator;
  std::allocator_traits<A>::pointer packet_buffer;
  std::size_t capacity;
  std::atomic_size_t head_index;
  std::atomic_size_t tail_index;
  std::atomic_size_t size;
  std::counting_semaphore<> send_ready;
  std::counting_semaphore<> recv_ready;
  std::atomic_size_t sender_count;
  std::atomic_size_t receiver_count;
  std::atomic_bool disconnected;

public:
  Channel(std::size_t capacity, A allocator)
      : allocator(std::move(allocator)),
        packet_buffer(
            std::allocator_traits<A>::allocate(this->allocator, capacity)),
        capacity(capacity), head_index(0), tail_index(0), size(0),
        send_ready(capacity), recv_ready(0), sender_count(1), receiver_count(1),
        disconnected(false) {
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
    if (this->disconnected.load(std::memory_order::relaxed)) {
      return false;
    }
    auto tail_index =
        this->tail_index.fetch_add(1, std::memory_order::relaxed) %
        this->capacity;
    if (tail_index == this->capacity - 1) {
      this->tail_index.fetch_sub(this->capacity, std::memory_order::relaxed);
    }
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
    auto size = this->size.fetch_sub(1, std::memory_order::relaxed);
    if (this->disconnected.load(std::memory_order::relaxed) && size == 0) {
      return {};
    }
    auto head_index =
        this->head_index.fetch_add(1, std::memory_order::relaxed) %
        this->capacity;
    if (head_index == this->capacity - 1) {
      this->head_index.fetch_sub(this->capacity, std::memory_order::relaxed);
    }
    auto &packet = this->packet_buffer[head_index];
    while (!packet.read_ready.exchange(false, std::memory_order::acquire)) {
      std::this_thread::yield();
    }
    auto item = std::move(packet.item);
    packet.write_ready.store(true, std::memory_order::release);
    return item;
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
} // namespace chan::mpmc::bounded
