#ifndef _CHAN_SPSC_UNBOUNDED_RECEIVER_H
#define _CHAN_SPSC_UNBOUNDED_RECEIVER_H

#include <memory>

#include "../../RecvIter.hpp"
#include "Channel.hpp"

namespace chan::spsc::unbounded {
/// Receiving half of a channel.
template <typename T, std::size_t CHUNK_SIZE = 16,
          typename A1 = std::allocator<ItemChunk<T, CHUNK_SIZE>>,
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

  /// Create a `Receiver` that is not connected to a channel.
  ///
  /// # Safety
  /// Most method calls on a default constructed `Receiver` result in undefined
  /// behavior. The only safe operations are assignment, destruction, and calls
  /// to `disconnect`.
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

  Receiver(const Receiver &) = delete;
  Receiver &operator=(const Receiver &) = delete;

  /// Receive an item from the channel.
  ///
  /// Blocks until the channel is not empty or the sender disconnects.
  std::expected<T, RecvError> recv() const { return this->channel->recv(); }

  /// Receive an item from the channel without blocking.
  ///
  /// Because semaphore try_acquire operations may spuriously fail, this
  /// function may spuriously fail with `TryRecvErrorKind::Empty`.
  std::expected<T, TryRecvError> try_recv() const {
    return this->channel->try_recv();
  }

  /// Receive an item from the channel with a timeout.
  ///
  /// Blocks until the channel is not empty, the timeout is met, or the sender
  /// disconnects.
  template <typename Rep, typename Period>
  std::expected<T, TryRecvError>
  try_recv_for(const std::chrono::duration<Rep, Period> &timeout) const {
    return this->channel->try_recv_for(timeout);
  }

  /// Receive an item from the channel with a deadline.
  ///
  /// Blocks until the channel is not empty, the deadline is met, or the sender
  /// disconnects.
  template <typename Clock, typename Duration>
  std::expected<T, TryRecvError> try_recv_until(
      const std::chrono::time_point<Clock, Duration> &deadline) const {
    return this->channel->try_recv_until(deadline);
  }

  /// Number of items in the channel.
  ///
  /// Since the channel's size could change at any moment, it should not be used
  /// to determine if a receive operation will block/fail.
  std::size_t channel_size() const {
    return this->channel->size.load(std::memory_order::relaxed);
  }

  /// Number of items the channel has allocated space for.
  std::size_t channel_capacity() const {
    return this->channel->capacity.load(std::memory_order::relaxed);
  }

  /// Disconnect from the channel.
  ///
  /// There is often no need to call this function because the destructor will
  /// do the same thing.
  ///
  /// # Safety
  /// After calling `disconnect`, a `Receiver` has the same safety rules as a
  /// default constructed `Receiver`.
  void disconnect() {
    this->release();
    this->channel = nullptr;
  }

private:
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
} // namespace chan::spsc::unbounded

#endif
