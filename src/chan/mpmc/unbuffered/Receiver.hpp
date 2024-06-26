#ifndef _CHAN_MPMC_UNBUFFERED_RECEIVER_H
#define _CHAN_MPMC_UNBUFFERED_RECEIVER_H

#include <cassert>
#include <memory>

#include "../../RecvIter.hpp"
#include "Chan.hpp"

namespace chan::mpmc::unbuffered {
/// Receiving half of a channel.
template <typename T, typename A1 = std::allocator<std::optional<T> *>,
          typename A2 = std::allocator<Chan<T, A1>>>
class Receiver {
public:
  using Item = T;

private:
  std::allocator_traits<A2>::pointer channel;
  A2 allocator;

public:
  Receiver(std::allocator_traits<A2>::pointer channel, A2 allocator)
      : channel(std::move(channel)), allocator(std::move(allocator)) {}

  /// Create a null `Receiver`.
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

  /// Return `true` if `this` is not connected to a channel.
  bool is_null() const { return this->channel == nullptr; }

  /// Return `!is_null()`.
  explicit operator bool() const { return !this->is_null(); }

  /// Receive an item from the channel.
  ///
  /// Blocks until a sender sends an item or all senders disconnect.
  ///
  /// # Safety
  /// Causes undefined behavior if `is_null()` is `true`.
  std::expected<T, RecvError> recv() const {
    assert(this->channel != nullptr);
    return this->channel->recv();
  }

  /// Receive an item from the channel without blocking.
  ///
  /// # Safety
  /// Causes undefined behavior if `is_null()` is `true`.
  std::expected<T, TryRecvError> try_recv() const {
    assert(this->channel != nullptr);
    return this->channel->try_recv();
  }

  /// Receive an item from the channel with a timeout.
  ///
  /// Blocks until a sender sends an item, the timeout is met, or all senders
  /// disconnect.
  ///
  /// # Safety
  /// Causes undefined behavior if `is_null()` is `true`.
  template <typename Rep, typename Period>
  std::expected<T, TryRecvError>
  try_recv_for(const std::chrono::duration<Rep, Period> &timeout) const {
    assert(this->channel != nullptr);
    return this->channel->try_recv_for(timeout);
  }

  /// Receive an item from the channel with a deadline.
  ///
  /// Blocks until a sender sends an item, the deadline is met, or all senders
  /// disconnect.
  ///
  /// # Safety
  /// Causes undefined behavior if `is_null()` is `true`.
  template <typename Clock, typename Duration>
  std::expected<T, TryRecvError> try_recv_until(
      const std::chrono::time_point<Clock, Duration> &deadline) const {
    assert(this->channel != nullptr);
    return this->channel->try_recv_until(deadline);
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
} // namespace chan::mpmc::unbuffered

#endif
