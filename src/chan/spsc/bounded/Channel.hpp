#ifndef _CHAN_SPSC_BOUNDED_CHANNEL_H
#define _CHAN_SPSC_BOUNDED_CHANNEL_H

#include <atomic>
#include <memory>
#include <optional>
#include <semaphore>

#include "../../detail/BoundedChannel.hpp"

namespace chan::spsc::bounded {
template <typename T, typename A>
class Channel : detail::BoundedChannel<Channel<T, A>, T> {
  friend struct detail::BoundedChannel<Channel, T>;
  template <typename, typename, typename> friend class Sender;
  template <typename, typename, typename> friend class Receiver;

  A allocator;
  std::allocator_traits<A>::pointer item_buffer;
  std::size_t capacity;
  std::size_t head_index;
  std::size_t tail_index;
  std::atomic_size_t size;
  std::counting_semaphore<> send_ready;
  std::counting_semaphore<> recv_ready;
  std::atomic_bool recv_done;
  std::atomic_bool disconnected;

public:
  Channel(std::size_t capacity, A allocator)
      : allocator(std::move(allocator)),
        item_buffer(
            std::allocator_traits<A>::allocate(this->allocator, capacity)),
        capacity(capacity), head_index(0), tail_index(0), size(0),
        send_ready(capacity), recv_ready(0), recv_done(false),
        disconnected(false) {}

  ~Channel() {
    auto index = this->head_index;
    while (index != this->tail_index) {
      std::allocator_traits<A>::destroy(this->allocator,
                                        this->item_buffer + index);
      if (++index == this->capacity) {
        index = 0;
      }
    }
    std::allocator_traits<A>::deallocate(this->allocator, this->item_buffer,
                                         this->capacity);
  }

private:
  bool send_impl(T &item) {
    if (this->recv_done.load(std::memory_order::relaxed)) {
      return false;
    }
    std::allocator_traits<A>::construct(
        this->allocator, this->item_buffer + this->tail_index, std::move(item));
    if (++this->tail_index == this->capacity) {
      this->tail_index = 0;
    }
    this->size.fetch_add(1, std::memory_order::relaxed);
    return true;
  }

  std::optional<T> recv_impl() {
    if (this->size.fetch_sub(1, std::memory_order::relaxed) == 0) {
      return {};
    }
    auto &chan_item = this->item_buffer[this->head_index];
    auto item = std::move(chan_item);
    std::allocator_traits<A>::destroy(this->allocator, &chan_item);
    if (++this->head_index == this->capacity) {
      this->head_index = 0;
    }
    return item;
  }

  bool release_sender() {
    this->recv_ready.release();
    return this->disconnected.exchange(true, std::memory_order::relaxed);
  }

  bool release_receiver() {
    this->recv_done.store(true, std::memory_order::relaxed);
    this->send_ready.release();
    return this->disconnected.exchange(true, std::memory_order::relaxed);
  }
};
} // namespace chan::spsc::bounded

#endif
