#ifndef _CHAN_SPSC_UNBOUNDED_CHANNEL_H
#define _CHAN_SPSC_UNBOUNDED_CHANNEL_H

#include <atomic>
#include <expected>
#include <memory>
#include <optional>

#include "../../SendError.hpp"
#include "../../detail/SemaphoreType.hpp"
#include "../../detail/UnboundedChannel.hpp"
#include "ItemChunk.hpp"

namespace chan::spsc::unbounded {
template <typename T, std::size_t CHUNK_SIZE, typename A>
  requires(CHUNK_SIZE != 0)
class Channel : detail::UnboundedChannel<Channel<T, CHUNK_SIZE, A>, T> {
  friend struct detail::UnboundedChannel<Channel, T>;
  template <typename, std::size_t, typename, typename> friend class Sender;
  template <typename, std::size_t, typename, typename> friend class Receiver;

  A allocator;
  ItemChunk<T, CHUNK_SIZE> *tail_chunk;
  std::size_t tail_index;
  ItemChunk<T, CHUNK_SIZE> *head_chunk;
  std::size_t head_index;
  std::atomic_size_t size;
  std::size_t capacity;
  detail::SemaphoreType recv_ready;
  std::atomic_bool send_done;
  std::atomic_bool disconnected;

public:
  Channel(A allocator)
      : allocator(std::move(allocator)),
        tail_chunk(std::allocator_traits<A>::allocate(this->allocator, 1)),
        tail_index(0), head_chunk(this->tail_chunk), head_index(0), size(0),
        capacity(CHUNK_SIZE), recv_ready(0), send_done(false),
        disconnected(false) {
    this->tail_chunk->next = this->tail_chunk;
  }

  ~Channel() {
    auto chunk = this->head_chunk;
    auto index = this->head_index;
    while (chunk != this->tail_chunk || index != this->tail_index) {
      std::allocator_traits<A>::destroy(this->allocator, chunk->items + index);
      if (++index == CHUNK_SIZE) {
        index = 0;
        chunk = chunk->next;
      }
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
      return std::unexpected(SendError{std::move(item)});
    }
    auto size = this->size.fetch_add(1, std::memory_order::acquire);
    std::allocator_traits<A>::construct(
        this->allocator, this->tail_chunk->items + this->tail_index,
        std::move(item));
    if (this->tail_index != CHUNK_SIZE - 1) {
      ++this->tail_index;
    } else {
      this->tail_index = 0;
      if (size < this->capacity - CHUNK_SIZE) {
        this->tail_chunk = this->tail_chunk->next;
      } else {
        auto new_chunk = std::allocator_traits<A>::allocate(this->allocator, 1);
        new_chunk->next = this->tail_chunk->next;
        this->tail_chunk->next = new_chunk;
        this->tail_chunk = new_chunk;
        this->capacity += CHUNK_SIZE;
      }
    }
    this->recv_ready.release();
    return {};
  }

  std::optional<T> recv_impl() {
    if (this->size.load(std::memory_order::relaxed) == 0) {
      return {};
    }
    auto &chan_item = this->head_chunk->items[this->head_index];
    auto item = std::move(chan_item);
    std::allocator_traits<A>::destroy(this->allocator, &chan_item);
    if (this->head_index != CHUNK_SIZE - 1) {
      ++this->head_index;
    } else {
      this->head_index = 0;
      this->head_chunk = this->head_chunk->next;
    }
    this->size.fetch_sub(1, std::memory_order::release);
    return item;
  }

  bool release_sender() {
    this->send_done.store(true, std::memory_order::relaxed);
    this->recv_ready.release();
    return this->disconnected.exchange(true, std::memory_order::relaxed);
  }

  bool release_receiver() {
    return this->disconnected.exchange(true, std::memory_order::relaxed);
  }
};
} // namespace chan::spsc::unbounded

#endif
