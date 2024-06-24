#ifndef _CHAN_MPMC_UNBOUNDED_RECEIVER_H
#define _CHAN_MPMC_UNBOUNDED_RECEIVER_H

#include <memory>

#include "../../RecvIter.hpp"
#include "Channel.hpp"

namespace chan::mpmc::unbounded {
template <typename T, std::size_t CHUNK_SIZE = 16,
          typename A1 = std::allocator<PacketChunk<T, CHUNK_SIZE>>,
          typename A2 = std::allocator<Channel<T, CHUNK_SIZE, A1>>>
class Receiver {
public:
  using Item = T;

private:
  std::allocator_traits<A2>::pointer channel;
  A2 allocator;

public:
  Receiver(std::allocator_traits<A2>::pointer channel, A2 allocator)
      : channel(std::move(channel)), allocator(std::move(allocator)) {}

  Receiver() : channel(nullptr), allocator() {}

  ~Receiver() { this->release(); }

  Receiver(Receiver &&other)
      : channel(std::move(other.channel)),
        allocator(std::move(other.allocator)) {
    other.channel = nullptr;
  }

  Receiver &operator=(Receiver &&other) {
    if (this != &other) {
      this->release();
      this->channel = std::move(other.channel);
      this->allocator = std::move(other.allocator);
      other.channel = nullptr;
    }
    return *this;
  }

  Receiver(const Receiver &other)
      : channel(other.channel), allocator(other.allocator) {
    this->acquire();
  }

  Receiver &operator=(const Receiver &other) {
    this->release();
    this->channel = other.channel;
    this->allocator = other.allocator;
    this->acquire();
    return *this;
  }

  std::expected<T, RecvError> recv() const { return this->channel->recv(); }

  std::expected<T, TryRecvError> try_recv() const {
    return this->channel->try_recv();
  }

  template <typename Rep, typename Period>
  std::expected<T, TryRecvError>
  try_recv_for(const std::chrono::duration<Rep, Period> &timeout) const {
    return this->channel->try_recv_for(timeout);
  }

  template <typename Clock, typename Duration>
  std::expected<T, TryRecvError> try_recv_until(
      const std::chrono::time_point<Clock, Duration> &deadline) const {
    return this->channel->try_recv_until(deadline);
  }

  std::size_t channel_size() {
    return this->channel->size.load(std::memory_order::relaxed);
  }

  std::size_t channel_capacity() {
    return this->channel->capacity.load(std::memory_order::relaxed);
  }

  void disconnect() {
    this->release();
    this->channel = nullptr;
  }

private:
  void acquire() {
    if (this->channel) {
      this->channel->acquire_receiver();
    }
  }

  void release() {
    if (this->channel && this->channel->release_receiver()) {
      std::allocator_traits<A2>::destroy(this->allocator, this->channel);
      std::allocator_traits<A2>::deallocate(this->allocator, this->channel, 1);
    }
  }

public:
  RecvIter<Receiver> begin() const { return RecvIter<Receiver>(*this); }

  std::default_sentinel_t end() const { return {}; }
};
} // namespace chan::mpmc::unbounded

#endif
