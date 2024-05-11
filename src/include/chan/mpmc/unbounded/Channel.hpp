#pragma once

#include <atomic>
#include <expected>
#include <memory>
#include <mutex>
#include <semaphore>
#include <thread>

#include "../../SendError.hpp"
#include "../../detail/UnboundedChannel.hpp"
#include "PacketChunk.hpp"

namespace chan::mpmc::unbounded {
template <typename T, std::size_t CHUNK_SIZE, typename A>
class Channel : detail::UnboundedChannel<Channel<T, CHUNK_SIZE, A>, T> {
  friend class detail::UnboundedChannel<Channel, T>;
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

  std::counting_semaphore<> recv_ready;

  std::atomic_size_t sender_count;
  std::atomic_size_t receiver_count;
  std::atomic_bool disconnected;

public:
  Channel(A allocator)
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

  ~Channel() {
    auto chunk = this->head_chunk;
    auto index = this->head_index;
    while (chunk != this->tail_chunk || index != this->tail_index) {
      if (index == CHUNK_SIZE) {
        index = 0;
        chunk = chunk->next;
      }
      std::allocator_traits<A>::destroy(this->allocator,
                                        chunk->packets + index);
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
    auto size = this->size.fetch_add(1, std::memory_order::relaxed);
    Packet<T> *packet;
    {
      std::lock_guard _lock(this->tail_position_mutex);
      if (this->tail_index == CHUNK_SIZE) {
        this->tail_index = 0;
        auto capacity = this->capacity.load(std::memory_order::relaxed);
        if (size <= capacity - CHUNK_SIZE) {
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
          this->capacity.store(capacity + CHUNK_SIZE,
                               std::memory_order::relaxed);
        }
      }
      packet = &this->tail_chunk->packets[this->tail_index];
      this->tail_index += 1;
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

  std::optional<T> recv_impl() {
    if (this->size.load(std::memory_order::relaxed) == 0) {
      return {};
    }
    Packet<T> *packet;
    {
      std::lock_guard _lock(this->head_position_mutex);
      if (this->head_index == CHUNK_SIZE) {
        this->head_index = 0;
        this->head_chunk = this->head_chunk->next;
      }
      packet = &this->head_chunk->packets[this->head_index];
      this->head_index += 1;
    }
    while (!packet->read_ready.exchange(false, std::memory_order::acquire)) {
      std::this_thread::yield();
    }
    auto item = std::move(packet->item);
    packet->write_ready.store(true, std::memory_order::release);
    this->size.fetch_sub(1, std::memory_order::relaxed);
    return item;
  }

  void acquire_sender() {
    this->sender_count.fetch_add(1, std::memory_order::relaxed);
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
    return this->disconnected.exchange(true, std::memory_order::relaxed);
  }
};
} // namespace chan::mpmc::unbounded
