#ifndef _CHAN_SPSC_UNBUFFERED_RECEIVER_H
#define _CHAN_SPSC_UNBUFFERED_RECEIVER_H

#include <memory>

#include "../../RecvIter.hpp"
#include "Channel.hpp"

namespace chan::spsc::unbuffered {
/// Receiving half of a channel.
template <typename T, typename A = std::allocator<Channel<T>>> class Receiver {
public:
  using Item = T;

private:
  std::allocator_traits<A>::pointer channel;
  A allocator;

public:
  Receiver(std::allocator_traits<A>::pointer channel, A allocator)
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
  /// Blocks until the sender sends an item or the sender disconnects.
  std::expected<T, RecvError> recv() const { return this->channel->recv(); }

  /// Receive an item from the channel without blocking.
  std::expected<T, TryRecvError> try_recv() const {
    return this->channel->try_recv();
  }

  /// Receive an item from the channel with a timeout.
  ///
  /// Blocks until the sender sends an item, the timeout is met, or the sender
  /// disconnects.
  template <typename Rep, typename Period>
  std::expected<T, TryRecvError>
  try_recv_for(const std::chrono::duration<Rep, Period> &timeout) const {
    return this->channel->try_recv_for(timeout);
  }

  /// Receive an item from the channel with a deadline.
  ///
  /// Blocks until the sender sends an item, the deadline is met, or the sender
  /// disconnects.
  template <typename Clock, typename Duration>
  std::expected<T, TryRecvError> try_recv_until(
      const std::chrono::time_point<Clock, Duration> &deadline) const {
    return this->channel->try_recv_until(deadline);
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
      std::allocator_traits<A>::destroy(this->allocator, this->channel);
      std::allocator_traits<A>::deallocate(this->allocator, this->channel, 1);
    }
  }

public:
  RecvIter<Receiver> begin() const { return RecvIter<Receiver>(*this); }

  std::default_sentinel_t end() const { return {}; }
};
} // namespace chan::spsc::unbuffered

#endif
