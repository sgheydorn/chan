#ifndef _CHAN_MPSC_UNBOUNDED_SENDER_H
#define _CHAN_MPSC_UNBOUNDED_SENDER_H

#include <memory>

#include "../../SendIter.hpp"
#include "Channel.hpp"

namespace chan::mpsc::unbounded {
template <typename T, std::size_t CHUNK_SIZE = 16,
          typename A1 = std::allocator<PacketChunk<T, CHUNK_SIZE>>,
          typename A2 = std::allocator<Channel<T, CHUNK_SIZE, A1>>>
class Sender {
public:
  using Item = T;

private:
  std::allocator_traits<A2>::pointer channel;
  A2 allocator;

public:
  Sender(std::allocator_traits<A2>::pointer channel, A2 allocator)
      : channel(std::move(channel)), allocator(std::move(allocator)) {}

  Sender() : channel(nullptr), allocator() {}

  ~Sender() { this->release(); }

  Sender(Sender &&other)
      : channel(std::move(other.channel)),
        allocator(std::move(other.allocator)) {
    other.channel = nullptr;
  }

  Sender &operator=(Sender &&other) {
    if (this != &other) {
      this->release();
      this->channel = std::move(other.channel);
      this->allocator = std::move(other.allocator);
      other.channel = nullptr;
    }
    return *this;
  }

  Sender(const Sender &other)
      : channel(other.channel), allocator(other.allocator) {
    this->acquire();
  }

  Sender &operator=(const Sender &other) {
    this->release();
    this->channel = other.channel;
    this->allocator = other.allocator;
    this->acquire();
    return *this;
  }

  std::expected<void, SendError<T>> send(T item) const {
    return this->channel->send(std::move(item));
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
      this->channel->acquire_sender();
    }
  }

  void release() {
    if (this->channel && this->channel->release_sender()) {
      std::allocator_traits<A2>::destroy(this->allocator, this->channel);
      std::allocator_traits<A2>::deallocate(this->allocator, this->channel, 1);
    }
  }

public:
  SendIter<Sender> begin() const { return SendIter(*this); }

  std::default_sentinel_t end() const { return {}; }
};
} // namespace chan::mpsc::unbounded

#endif
