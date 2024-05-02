#pragma once

#include <atomic>
#include <expected>
#include <memory>
#include <optional>
#include <semaphore>

#include "../../SendError.hpp"
#include "../../TryRecvError.hpp"
#include "../../TrySendError.hpp"

namespace chan::spsc::bounded {
template <typename T, typename A> class Channel {
  template <typename, typename, typename> friend class Sender;
  template <typename, typename, typename> friend class Receiver;

  A allocator;
  std::allocator_traits<A>::pointer item_buffer;
  std::size_t capacity;
  std::size_t head_index;
  std::size_t tail_index;
  std::atomic_size_t size;
  std::counting_semaphore<> items_available;
  std::counting_semaphore<> space_available;
  std::atomic_bool disconnected;

public:
  Channel(std::size_t capacity, A allocator)
      : allocator(std::move(allocator)),
        item_buffer(
            std::allocator_traits<A>::allocate(this->allocator, capacity)),
        capacity(capacity), head_index(0), tail_index(0), size(0),
        items_available(0), space_available(capacity), disconnected(false) {}

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
  std::expected<void, SendError<T>> send(T item) {
    this->space_available.acquire();
    if (!this->send_impl(item)) {
      return std::unexpected(SendError(std::move(item)));
    }
    return {};
  }

  std::expected<void, TrySendError<T>> try_send(T item) {
    if (!this->space_available.try_acquire()) {
      return std::unexpected(
          TrySendError(TrySendErrorKind::Full, std::move(item)));
    }
    return this->try_send_impl(std::move(item));
  }

  template <typename Rep, typename Period>
  std::expected<void, TrySendError<T>>
  try_send_for(T item, const std::chrono::duration<Rep, Period> &timeout) {
    if (!this->space_available.try_acquire_for(timeout)) {
      return std::unexpected(
          TrySendError(TrySendErrorKind::Full, std::move(item)));
    }
    return this->try_send_impl(std::move(item));
  }

  template <typename Clock, typename Duration>
  std::expected<void, TrySendError<T>>
  try_send_until(T item,
                 const std::chrono::time_point<Clock, Duration> &deadline) {
    if (!this->space_available.try_acquire_until(deadline)) {
      return std::unexpected(
          TrySendError(TrySendErrorKind::Full, std::move(item)));
    }
    return this->try_send_impl(std::move(item));
  }

  std::expected<void, TrySendError<T>> try_send_impl(T &&item) {
    if (!this->send_impl(item)) {
      return std::unexpected(
          TrySendError(TrySendErrorKind::Disconnected, std::move(item)));
    }
    return {};
  }

  bool send_impl(T &item) {
    if (this->disconnected.load(std::memory_order::relaxed)) {
      return false;
    }
    std::allocator_traits<A>::construct(
        this->allocator, this->item_buffer + this->tail_index, std::move(item));
    if (++this->tail_index == this->capacity) {
      this->tail_index = 0;
    }
    this->size.fetch_add(1, std::memory_order::relaxed);
    this->items_available.release();
    return true;
  }

  std::expected<T, RecvError> recv() {
    this->items_available.acquire();
    auto item = this->recv_impl();
    if (!item) {
      return std::unexpected(RecvError());
    }
    return std::move(*item);
  }

  std::expected<T, TryRecvError> try_recv() {
    if (!this->items_available.try_acquire()) {
      return std::unexpected(TryRecvError(TryRecvErrorKind::Empty));
    }
    return this->try_recv_impl();
  }

  template <typename Rep, typename Period>
  std::expected<T, TryRecvError>
  try_recv_for(const std::chrono::duration<Rep, Period> &timeout) {
    if (!this->items_available.try_acquire_for(timeout)) {
      return std::unexpected(TryRecvError(TryRecvErrorKind::Empty));
    }
    return this->try_recv_impl();
  }

  template <typename Clock, typename Duration>
  std::expected<T, TryRecvError>
  try_recv_until(const std::chrono::duration<Clock, Duration> &deadline) {
    if (!this->items_available.try_acquire_until(deadline)) {
      return std::unexpected(TryRecvError(TryRecvErrorKind::Empty));
    }
    return this->try_recv_impl();
  }

  std::expected<T, TryRecvError> try_recv_impl() {
    auto item = this->recv_impl();
    if (!item) {
      return std::unexpected(TryRecvError(TryRecvErrorKind::Disconnected));
    }
    return std::move(*item);
  }

  std::optional<T> recv_impl() {
    auto size = this->size.fetch_sub(1, std::memory_order::relaxed);
    if (this->disconnected.load(std::memory_order::relaxed) && size == 0) {
      return {};
    }
    auto item = std::move(this->item_buffer[this->head_index]);
    if (++this->head_index == this->capacity) {
      this->head_index = 0;
    }
    this->space_available.release();
    return item;
  }

  bool release_sender() {
    auto destroy =
        this->disconnected.exchange(true, std::memory_order::relaxed);
    this->items_available.release();
    return destroy;
  }

  bool release_receiver() {
    auto destroy =
        this->disconnected.exchange(true, std::memory_order::relaxed);
    this->space_available.release();
    return destroy;
  }
};
} // namespace chan::spsc::bounded
