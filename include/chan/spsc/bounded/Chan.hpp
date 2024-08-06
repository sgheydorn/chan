#ifndef _CHAN_SPSC_BOUNDED_CHANNEL_H
#define _CHAN_SPSC_BOUNDED_CHANNEL_H

#include <atomic>
#include <memory>
#include <optional>

#include "../../detail/BoundedChannel.hpp"
#include "../../detail/SemaphoreType.hpp"

namespace chan::spsc::bounded {
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
  std::allocator_traits<A>::pointer item_buffer;
  std::size_t capacity;
  std::size_t head_index;
  std::size_t tail_index;
  std::atomic_size_t size;
  detail::SemaphoreType send_ready;
  detail::SemaphoreType recv_ready;
  std::atomic_bool _send_done;
  std::atomic_bool _recv_done;
  std::atomic_bool disconnected;

public:
  /// Create a channel that assumes a single `Sender` and single `Receiver`.
  ///
  /// This constructor should not be called directly. Instead, call the
  /// `channel` function.
  Chan(std::size_t capacity, A allocator)
      : allocator(std::move(allocator)),
        item_buffer(
            std::allocator_traits<A>::allocate(this->allocator, capacity)),
        capacity(capacity), head_index(0), tail_index(0), size(0),
        send_ready(capacity), recv_ready(0), _send_done(false),
        _recv_done(false), disconnected(false) {}

  ~Chan() {
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
  void do_send(T item) {
    std::allocator_traits<A>::construct(
        this->allocator, this->item_buffer + this->tail_index, std::move(item));
    if (++this->tail_index == this->capacity) {
      this->tail_index = 0;
    }
  }

  T do_recv() {
    auto &chan_item = this->item_buffer[this->head_index];
    auto item = std::move(chan_item);
    std::allocator_traits<A>::destroy(this->allocator, &chan_item);
    if (++this->head_index == this->capacity) {
      this->head_index = 0;
    }
    return item;
  }

  bool send_done() const {
    return this->_send_done.load(std::memory_order::acquire);
  }

  bool recv_done() const {
    return this->_recv_done.load(std::memory_order::acquire);
  }

  bool release_sender() {
    this->_send_done.store(true, std::memory_order::release);
    this->recv_ready.release();
    return this->disconnected.exchange(true, std::memory_order::acq_rel);
  }

  bool release_receiver() {
    this->_recv_done.store(true, std::memory_order::release);
    this->send_ready.release();
    return this->disconnected.exchange(true, std::memory_order::acq_rel);
  }
};
} // namespace chan::spsc::bounded

#endif
