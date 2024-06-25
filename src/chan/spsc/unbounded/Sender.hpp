#ifndef _CHAN_SPSC_UNBOUNDED_SENDER_H
#define _CHAN_SPSC_UNBOUNDED_SENDER_H

#include <memory>

#include "../../SendIter.hpp"
#include "Channel.hpp"

namespace chan::spsc::unbounded {
/// Sending half of a channel.
template <typename T, std::size_t CHUNK_SIZE = 16,
          typename A1 = std::allocator<ItemChunk<T, CHUNK_SIZE>>,
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

  /// Create a null `Sender`.
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

  Sender(const Sender &) = delete;
  Sender &operator=(const Sender &) = delete;

  /// Return `true` if `this` is not connected to a channel.
  bool is_null() const { return this->channel == nullptr; }

  /// Return `!is_null()`.
  explicit operator bool() const { return !this->is_null(); }

  /// Send an item on the channel.
  ///
  /// Does not block.
  ///
  /// # Safety
  /// Causes undefined behavior if `is_null()` is `true`.
  std::expected<void, SendError<T>> send(T item) const {
    return this->channel->send(std::move(item));
  }

  /// Number of items in the channel.
  ///
  /// # Safety
  /// Causes undefined behavior if `is_null()` is `true`.
  std::size_t channel_size() const {
    return this->channel->size.load(std::memory_order::relaxed);
  }

  /// Number of items the channel has allocated space for.
  ///
  /// # Safety
  /// Causes undefined behavior if `is_null()` is `true`.
  std::size_t channel_capacity() const {
    return this->channel->capacity.load(std::memory_order::relaxed);
  }

  /// Disconnect from the channel.
  ///
  /// There is often no need to call this function because the destructor will
  /// disconnect from the channel.
  ///
  /// After calling this function, `is_null()` will be `true`.
  void disconnect() {
    this->release();
    this->channel = nullptr;
  }

private:
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
} // namespace chan::spsc::unbounded

#endif
