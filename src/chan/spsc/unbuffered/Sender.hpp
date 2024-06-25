#ifndef _CHAN_SPSC_UNBUFFERED_SENDER_H
#define _CHAN_SPSC_UNBUFFERED_SENDER_H

#include <memory>

#include "../../SendIter.hpp"
#include "Channel.hpp"

namespace chan::spsc::unbuffered {
/// Sending half of a channel.
template <typename T, typename A = std::allocator<Channel<T>>> class Sender {
public:
  using Item = T;

private:
  std::allocator_traits<A>::pointer channel;
  A allocator;

public:
  Sender(std::allocator_traits<A>::pointer channel, A allocator)
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

  /// Send an item on the channel.
  ///
  /// Blocks until the receiver requests an item or the receiver disconnects.
  std::expected<void, SendError<T>> send(T item) const {
    return this->channel->send(std::move(item));
  }

  /// Send an item on the channel without blocking.
  std::expected<void, TrySendError<T>> try_send(T item) const {
    return this->channel->try_send(std::move(item));
  }

  /// Send an item on the channel with a timeout.
  ///
  /// Blocks until the receiver requests an item, the timeout is met, or the
  /// receiver disconnects.
  template <typename Rep, typename Period>
  std::expected<void, TrySendError<T>>
  try_send_for(T item,
               const std::chrono::duration<Rep, Period> &timeout) const {
    return this->channel->try_send_for(std::move(item), timeout);
  }

  /// Send an item on the channel with a deadline.
  ///
  /// Blocks until the receiver requests an item, the deadline is met, or
  /// the receiver disconnects.
  template <typename Clock, typename Duration>
  std::expected<void, TrySendError<T>> try_send_until(
      T item, const std::chrono::time_point<Clock, Duration> &deadline) const {
    return this->channel->try_send_until(std::move(item), deadline);
  }

  /// Disconnect from the channel.
  ///
  /// There is often no need to call this function because the destructor will
  /// do the same thing.
  ///
  /// # Safety
  /// After calling `disconnect`, a `Sender` has the same safety rules as a
  /// default constructed `Sender`.
  void disconnect() {
    this->release();
    this->channel = nullptr;
  }

private:
  void release() {
    if (this->channel && this->channel->release_sender()) {
      std::allocator_traits<A>::destroy(this->allocator, this->channel);
      std::allocator_traits<A>::deallocate(this->allocator, this->channel, 1);
    }
  }

public:
  SendIter<Sender> begin() const { return SendIter(*this); }

  std::default_sentinel_t end() const { return {}; }
};
} // namespace chan::spsc::unbuffered

#endif
