#pragma once

#include <atomic>
#include <expected>
#include <memory>
#include <optional>
#include <semaphore>

#include "../../RecvError.hpp"
#include "../../SendError.hpp"
#include "../../TryRecvError.hpp"
#include "ItemChunk.hpp"

namespace chan::spsc::unbounded {
template <typename T, std::size_t CHUNK_SIZE, typename A> class Channel {
  template <typename, std::size_t, typename, typename> friend class Sender;
  template <typename, std::size_t, typename, typename> friend class Receiver;

  A allocator;
  ItemChunk<T, CHUNK_SIZE> *tail_chunk;
  std::size_t tail_index;
  ItemChunk<T, CHUNK_SIZE> *head_chunk;
  std::size_t head_index;
  std::atomic_size_t size;
  std::size_t capacity;
  std::counting_semaphore<> items_available;
  std::atomic_bool disconnected;

public:
  Channel(A allocator)
      : allocator(std::move(allocator)),
        tail_chunk(std::allocator_traits<A>::allocate(this->allocator, 1)),
        tail_index(0), head_chunk(this->tail_chunk), head_index(0), size(0),
        capacity(CHUNK_SIZE), items_available(0), disconnected(false) {
    this->tail_chunk->next = this->tail_chunk;
  }

  ~Channel() {
    auto chunk = this->head_chunk;
    auto index = this->head_index;
    while (chunk != this->tail_chunk || index != this->tail_index) {
      if (index == CHUNK_SIZE) {
        index = 0;
        chunk = chunk->next;
      }
      std::allocator_traits<A>::destroy(this->allocator, chunk->items + index);
      index += 1;
    }

    auto c = chunk->next;
    std::allocator_traits<A>::deallocate(this->allocator, chunk, 1);
    while (c != chunk) {
      auto next = c->next;
      std::allocator_traits<A>::deallocate(this->allocator, c, 1);
      c = next;
    }
  }

private:
  std::expected<void, SendError<T>> send(T item) {
    if (this->disconnected.load(std::memory_order::relaxed)) {
      return std::unexpected(SendError(std::move(item)));
    }
    auto size = this->size.fetch_add(1, std::memory_order::acquire);
    if (this->tail_index == CHUNK_SIZE) {
      this->tail_index = 0;
      if (size <= this->capacity - CHUNK_SIZE) {
        this->tail_chunk = this->tail_chunk->next;
      } else {
        auto new_chunk = std::allocator_traits<A>::allocate(this->allocator, 1);
        new_chunk->next = this->tail_chunk->next;
        this->tail_chunk->next = new_chunk;
        this->tail_chunk = new_chunk;
        this->capacity += CHUNK_SIZE;
      }
    }
    std::allocator_traits<A>::construct(
        this->allocator, this->tail_chunk->items + this->tail_index,
        std::move(item));
    this->tail_index += 1;
    this->items_available.release();
    return {};
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
    if (this->disconnected.load(std::memory_order::relaxed) &&
        this->size.load(std::memory_order::relaxed) == 0) {
      return {};
    }
    if (this->head_index == CHUNK_SIZE) {
      this->head_index = 0;
      this->head_chunk = this->head_chunk->next;
    }
    auto item = std::move(this->head_chunk->items[this->head_index]);
    this->head_index += 1;
    this->size.fetch_sub(1, std::memory_order::relaxed);
    return item;
  }

  bool release_sender() {
    auto destroy =
        this->disconnected.exchange(true, std::memory_order::relaxed);
    this->items_available.release();
    return destroy;
  }

  bool release_receiver() {
    return this->disconnected.exchange(true, std::memory_order::relaxed);
  }
};
} // namespace chan::spsc::unbounded
