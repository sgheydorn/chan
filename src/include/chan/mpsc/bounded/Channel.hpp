#pragma once

#include <atomic>
#include <expected>
#include <memory>
#include <semaphore>
#include <thread>

#include "../../RecvError.hpp"
#include "../../SendError.hpp"
#include "../../TryRecvError.hpp"
#include "../../TrySendError.hpp"
#include "../Packet.hpp"

namespace chan::mpsc::bounded {
template <typename T, typename A> class Channel {
  template <typename, typename, typename> friend class Sender;
  template <typename, typename, typename> friend class Receiver;

  A allocator;
  std::allocator_traits<A>::pointer packet_buffer;
  std::size_t capacity;
  std::size_t head_index;
  std::atomic_size_t tail_index;
  std::atomic_size_t size;
  std::counting_semaphore<> packets_available;
  std::counting_semaphore<> space_available;
  std::atomic_size_t sender_count;
  std::atomic_bool disconnected;

public:
  Channel(std::size_t capacity, A allocator)
      : allocator(std::move(allocator)),
        packet_buffer(
            std::allocator_traits<A>::allocate(this->allocator, capacity)),
        capacity(capacity), head_index(0), tail_index(0), size(0),
        packets_available(0), space_available(capacity), sender_count(1),
        disconnected(false) {
    for (std::size_t index = 0; index < capacity; ++index) {
      std::allocator_traits<A>::construct(
          this->allocator, &this->packet_buffer[index].ready, false);
    }
  }

  ~Channel() {
    auto index = this->head_index;
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
  std::expected<void, SendError<T>> send(T item) {
    this->space_available.acquire();
    if (this->disconnected.load(std::memory_order::relaxed)) {
      return std::unexpected(SendError(std::move(item)));
    }
    auto tail_index_nomod =
        this->tail_index.fetch_add(1, std::memory_order::relaxed);
    auto tail_index = tail_index_nomod % this->capacity;
    if (tail_index == 0 && tail_index_nomod != 0) {
      this->tail_index.fetch_sub(this->capacity, std::memory_order::relaxed);
    }
    auto &packet = this->packet_buffer[tail_index];
    std::allocator_traits<A>::construct(this->allocator, &packet.item,
                                        std::move(item));
    packet.ready.store(true, std::memory_order::release);
    this->size.fetch_add(1, std::memory_order::relaxed);
    this->packets_available.release();
    return {};
  }

  std::expected<void, TrySendError<T>> try_send(T item) {
    if (!this->space_available.try_acquire()) {
      return std::unexpected(
          TrySendError(TrySendErrorKind::Full, std::move(item)));
    }
    if (this->disconnected.load(std::memory_order::relaxed)) {
      return std::unexpected(
          TrySendError(TrySendErrorKind::Disconnected, std::move(item)));
    }
    auto tail_index_nomod =
        this->tail_index.fetch_add(1, std::memory_order::relaxed);
    auto tail_index = tail_index_nomod % this->capacity;
    if (tail_index == 0 && tail_index_nomod != 0) {
      this->tail_index.fetch_sub(this->capacity, std::memory_order::relaxed);
    }
    auto &packet = this->packet_buffer[tail_index];
    std::allocator_traits<A>::construct(this->allocator, &packet.item,
                                        std::move(item));
    packet.ready.store(true, std::memory_order::release);
    this->size.fetch_add(1, std::memory_order::relaxed);
    this->packets_available.release();
    return {};
  }

  template <typename Rep, typename Period>
  std::expected<void, TrySendError<T>>
  try_send_for(T item, const std::chrono::duration<Rep, Period> &timeout) {
    if (!this->space_available.try_acquire_for(timeout)) {
      return std::unexpected(
          TrySendError(TrySendErrorKind::Full, std::move(item)));
    }
    if (this->disconnected.load(std::memory_order::relaxed)) {
      return std::unexpected(
          TrySendError(TrySendErrorKind::Disconnected, std::move(item)));
    }
    auto tail_index_nomod =
        this->tail_index.fetch_add(1, std::memory_order::relaxed);
    auto tail_index = tail_index_nomod % this->capacity;
    if (tail_index == 0 && tail_index_nomod != 0) {
      this->tail_index.fetch_sub(this->capacity, std::memory_order::relaxed);
    }
    auto &packet = this->packet_buffer[tail_index];
    std::allocator_traits<A>::construct(this->allocator, &packet.item,
                                        std::move(item));
    packet.ready.store(true, std::memory_order::release);
    this->size.fetch_add(1, std::memory_order::relaxed);
    this->packets_available.release();
    return {};
  }

  template <typename Clock, typename Duration>
  std::expected<void, TrySendError<T>>
  try_send_until(T item,
                 const std::chrono::time_point<Clock, Duration> &deadline) {
    if (!this->space_available.try_acquire_until(deadline)) {
      return std::unexpected(
          TrySendError(TrySendErrorKind::Full, std::move(item)));
    }
    if (this->disconnected.load(std::memory_order::relaxed)) {
      return std::unexpected(
          TrySendError(TrySendErrorKind::Disconnected, std::move(item)));
    }
    auto tail_index_nomod =
        this->tail_index.fetch_add(1, std::memory_order::relaxed);
    auto tail_index = tail_index_nomod % this->capacity;
    if (tail_index == 0 && tail_index_nomod != 0) {
      this->tail_index.fetch_sub(this->capacity, std::memory_order::relaxed);
    }
    auto &packet = this->packet_buffer[tail_index];
    std::allocator_traits<A>::construct(this->allocator, &packet.item,
                                        std::move(item));
    packet.ready.store(true, std::memory_order::release);
    this->size.fetch_add(1, std::memory_order::relaxed);
    this->packets_available.release();
    return {};
  }

  std::expected<T, RecvError> recv() {
    this->packets_available.acquire();
    auto size = this->size.fetch_sub(1, std::memory_order::relaxed);
    if (this->disconnected.load(std::memory_order::relaxed) && size == 0) {
      return std::unexpected(RecvError());
    }
    auto &packet = this->packet_buffer[this->head_index];
    while (!packet.ready.exchange(false, std::memory_order::acquire)) {
      std::this_thread::yield();
    }
    auto item = std::move(packet.item);
    if (++this->head_index == this->capacity) {
      this->head_index = 0;
    }
    this->space_available.release();
    return item;
  }

  std::expected<T, TryRecvError> try_recv() {
    if (!this->packets_available.try_acquire()) {
      return std::unexpected(TryRecvError(TryRecvErrorKind::Empty));
    }
    auto size = this->size.fetch_sub(1, std::memory_order::relaxed);
    if (this->disconnected.load(std::memory_order::relaxed) && size == 0) {
      return std::unexpected(TryRecvError(TryRecvErrorKind::Disconnected));
    }
    auto &packet = this->packet_buffer[this->head_index];
    while (!packet.ready.exchange(false, std::memory_order::acquire)) {
      std::this_thread::yield();
    }
    auto item = std::move(packet.item);
    if (++this->head_index == this->capacity) {
      this->head_index = 0;
    }
    this->space_available.release();
    return item;
  }

  template <typename Rep, typename Period>
  std::expected<T, TryRecvError>
  try_recv_for(const std::chrono::duration<Rep, Period> &timeout) {
    if (!this->packets_available.try_acquire_for(timeout)) {
      return std::unexpected(TryRecvError(TryRecvErrorKind::Empty));
    }
    auto size = this->size.fetch_sub(1, std::memory_order::relaxed);
    if (this->disconnected.load(std::memory_order::relaxed) && size == 0) {
      return std::unexpected(TryRecvError(TryRecvErrorKind::Disconnected));
    }
    auto &packet = this->packet_buffer[this->head_index];
    while (!packet.ready.exchange(false, std::memory_order::acquire)) {
      std::this_thread::yield();
    }
    auto item = std::move(packet.item);
    if (++this->head_index == this->capacity) {
      this->head_index = 0;
    }
    this->space_available.release();
    return item;
  }

  template <typename Clock, typename Duration>
  std::expected<T, TryRecvError>
  try_recv_until(const std::chrono::duration<Clock, Duration> &deadline) {
    if (!this->packets_available.try_acquire_until(deadline)) {
      return std::unexpected(TryRecvError(TryRecvErrorKind::Empty));
    }
    auto size = this->size.fetch_sub(1, std::memory_order::relaxed);
    if (this->disconnected.load(std::memory_order::relaxed) && size == 0) {
      return std::unexpected(TryRecvError(TryRecvErrorKind::Disconnected));
    }
    auto &packet = this->packet_buffer[this->head_index];
    while (!packet.ready.exchange(false, std::memory_order::acquire)) {
      std::this_thread::yield();
    }
    auto item = std::move(packet.item);
    if (++this->head_index == this->capacity) {
      this->head_index = 0;
    }
    this->space_available.release();
    return item;
  }

  bool acquire_sender() {
    if (this->disconnected.load(std::memory_order::relaxed)) {
      return false;
    }
    this->sender_count.fetch_add(1, std::memory_order::relaxed);
    return true;
  }

  bool release_sender() {
    if (this->sender_count.fetch_sub(1, std::memory_order::acq_rel) == 1) {
      this->packets_available.release();
      return this->disconnected.exchange(true, std::memory_order::relaxed);
    }
    return false;
  }

  bool release_receiver() {
    auto destroy =
        this->disconnected.exchange(true, std::memory_order::relaxed);
    auto sender_count = this->sender_count.load(std::memory_order::relaxed);
    this->space_available.release(sender_count * 2);
    return destroy;
  }
};
} // namespace chan::mpsc::bounded
