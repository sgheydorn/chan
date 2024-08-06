#ifndef _CHAN_MPMC_UNBOUNDED_CHANNEL_H
#define _CHAN_MPMC_UNBOUNDED_CHANNEL_H

#include <atomic>
#include <expected>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>

#include "../../SendError.hpp"
#include "../../detail/SemaphoreType.hpp"
#include "../../detail/UnboundedChannel.hpp"
#include "PacketChunk.hpp"

namespace chan::mpmc::unbounded {
/// Channel implementation.
///
/// Aside from custom allocators, there is no reason to work with this class
/// directly.
template <typename T, std::size_t CHUNK_SIZE, typename A>
  requires(CHUNK_SIZE != 0)
class Chan : detail::UnboundedChannel<Chan<T, CHUNK_SIZE, A>, T> {
  friend struct detail::UnboundedChannel<Chan, T>;
  template <typename, std::size_t, typename, typename> friend class Sender;
  template <typename, std::size_t, typename, typename> friend class Receiver;

  A allocator;

  PacketChunk<T, CHUNK_SIZE> *tail_chunk;
  std::size_t tail_index;
  std::mutex tail_position_mutex;

  PacketChunk<T, CHUNK_SIZE> *head_chunk;
  std::size_t head_index;
  std::mutex head_position_mutex;

  std::atomic_size_t size;
  std::atomic_size_t capacity;

  detail::SemaphoreType recv_ready;

  std::atomic_size_t sender_count;
  std::atomic_size_t receiver_count;
  std::atomic_bool disconnected;

public:
  /// Create a channel that assumes a single `Sender` and single `Receiver`.
  ///
  /// This constructor should not be called directly. Instead, call the
  /// `channel` function.
  Chan(A allocator)
      : allocator(std::move(allocator)),
        tail_chunk(std::allocator_traits<A>::allocate(this->allocator, 1)),
        tail_index(0), head_chunk(this->tail_chunk), head_index(0), size(0),
        capacity(CHUNK_SIZE), recv_ready(0), sender_count(1), receiver_count(1),
        disconnected(false) {
    for (auto &packet : this->tail_chunk->packets) {
      std::allocator_traits<A>::construct(this->allocator, &packet.read_ready,
                                          false);
      std::allocator_traits<A>::construct(this->allocator, &packet.write_ready,
                                          true);
    }
    this->tail_chunk->next = this->tail_chunk;
  }

  ~Chan() {
    auto chunk = this->head_chunk;
    auto index = this->head_index;
    while (chunk != this->tail_chunk || index != this->tail_index) {
      std::allocator_traits<A>::destroy(this->allocator,
                                        &chunk->packets[index].item);
      if (++index == CHUNK_SIZE) {
        index = 0;
        chunk = chunk->next;
      }
    }

    auto c = chunk->next;
    for (auto &packet : chunk->packets) {
      std::allocator_traits<A>::destroy(this->allocator, &packet.read_ready);
      std::allocator_traits<A>::destroy(this->allocator, &packet.write_ready);
    }
    std::allocator_traits<A>::deallocate(this->allocator, chunk, 1);
    while (c != chunk) {
      auto next = c->next;
      for (auto &packet : c->packets) {
        std::allocator_traits<A>::destroy(this->allocator, &packet.read_ready);
        std::allocator_traits<A>::destroy(this->allocator, &packet.write_ready);
      }
      std::allocator_traits<A>::deallocate(this->allocator, c, 1);
      c = next;
    }
  }

private:
  std::expected<void, SendError<T>> send(T item) {
    if (this->disconnected.load(std::memory_order::relaxed)) {
      return std::unexpected(SendError{std::move(item)});
    }
    Packet<T> *packet;
    {
      std::lock_guard _lock(this->tail_position_mutex);
      auto size = this->size.fetch_add(1, std::memory_order::acquire);
      packet = &this->tail_chunk->packets[this->tail_index];
      if (this->tail_index != CHUNK_SIZE - 1) {
        ++this->tail_index;
      } else {
        this->tail_index = 0;
        if (size <
            this->capacity.load(std::memory_order::relaxed) - CHUNK_SIZE) {
          this->tail_chunk = this->tail_chunk->next;
        } else {
          auto new_chunk =
              std::allocator_traits<A>::allocate(this->allocator, 1);
          for (auto &packet : new_chunk->packets) {
            std::allocator_traits<A>::construct(this->allocator,
                                                &packet.read_ready, false);
            std::allocator_traits<A>::construct(this->allocator,
                                                &packet.write_ready, true);
          }
          new_chunk->next = this->tail_chunk->next;
          this->tail_chunk->next = new_chunk;
          this->tail_chunk = new_chunk;
          this->capacity.fetch_add(CHUNK_SIZE, std::memory_order::relaxed);
        }
      }
    }
    while (!packet->write_ready.exchange(false, std::memory_order::acquire)) {
      std::this_thread::yield();
    }
    std::allocator_traits<A>::construct(this->allocator, &packet->item,
                                        std::move(item));
    packet->read_ready.store(true, std::memory_order::release);
    this->recv_ready.release();
    return {};
  }

  std::optional<T> do_recv() {
    Packet<T> *packet;
    {
      std::lock_guard _lock(this->head_position_mutex);
      if (this->send_done() && this->head_chunk == this->tail_chunk &&
          this->head_index == this->tail_index) {
        return {};
      }
      packet = &this->head_chunk->packets[this->head_index];
      if (this->head_index != CHUNK_SIZE - 1) {
        ++this->head_index;
      } else {
        this->head_index = 0;
        this->head_chunk = this->head_chunk->next;
      }
    }
    while (!packet->read_ready.exchange(false, std::memory_order::acquire)) {
      std::this_thread::yield();
    }
    auto item = std::move(packet->item);
    std::allocator_traits<A>::destroy(this->allocator, &packet->item);
    packet->write_ready.store(true, std::memory_order::release);
    this->size.fetch_sub(1, std::memory_order::release);
    return item;
  }

  bool send_done() const {
    return this->sender_count.load(std::memory_order::acquire) == 0;
  }

  void acquire_sender() {
    this->sender_count.fetch_add(1, std::memory_order::relaxed);
  }

  void acquire_receiver() {
    this->receiver_count.fetch_add(1, std::memory_order::relaxed);
  }

  bool release_sender() {
    if (this->sender_count.fetch_sub(1, std::memory_order::acq_rel) != 1) {
      return false;
    }
    auto receiver_count = this->receiver_count.load(std::memory_order::relaxed);
    this->recv_ready.release(receiver_count);
    return this->disconnected.exchange(true, std::memory_order::acq_rel);
  }

  bool release_receiver() {
    if (this->receiver_count.fetch_sub(1, std::memory_order::acq_rel) != 1) {
      return false;
    }
    return this->disconnected.exchange(true, std::memory_order::acq_rel);
  }
};
} // namespace chan::mpmc::unbounded

#endif
