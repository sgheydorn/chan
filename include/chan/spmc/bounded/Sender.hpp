#ifndef _CHAN_SPMC_BOUNDED_SENDER_H
#define _CHAN_SPMC_BOUNDED_SENDER_H

#include <cassert>
#include <memory>

#include "../../SendIter.hpp"
#include "Chan.hpp"

namespace chan::spmc::bounded {
/// Sending half of a channel.
///
/// # Template parameters
/// `T` - Channel's item type
/// `A1` (optional) - Allocator for the channel's item buffer
/// `A2` (optional) - Allocator for the channel object
///
/// # Safety
/// Do not share a `Sender` between threads. If multiple threads need to
/// send to the same channel, use mpmc instead of spmc.
template <typename T, typename A1 = std::allocator<Packet<T>>,
          typename A2 = std::allocator<Chan<T, A1>>>
class Sender {
public:
  using Item = T;

private:
  std::allocator_traits<A2>::pointer channel;
  A2 allocator;

public:
  /// Create the initial `Sender` for a channel.
  ///
  /// This constructor should not be called directly. Instead, call the
  /// `channel` function.
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

  Sender(const Sender &other) = delete;
  Sender &operator=(const Sender &other) = delete;

  /// Return `true` if `this` is not connected to a channel.
  bool is_null() const { return this->channel == nullptr; }

  /// Return `!is_null()`.
  explicit operator bool() const { return !this->is_null(); }

  /// Send an item on the channel.
  ///
  /// Blocks until the channel is not full or all receivers disconnect.
  ///
  /// # Safety
  /// Causes undefined behavior if `is_null()` is `true`.
  std::expected<void, SendError<T>> send(T item) const {
    assert(this->channel != nullptr);
    return this->channel->send(std::move(item));
  }

  /// Send an item on the channel without blocking.
  ///
  /// Because semaphore try_acquire operations may spuriously fail, this
  /// function may spuriously fail with `TrySendErrorKind::Full`.
  ///
  /// # Safety
  /// Causes undefined behavior if `is_null()` is `true`.
  std::expected<void, TrySendError<T>> try_send(T item) const {
    assert(this->channel != nullptr);
    return this->channel->try_send(std::move(item));
  }

  /// Send an item on the channel with a timeout.
  ///
  /// Blocks until the channel is not full, the timeout is met, or all receivers
  /// disconnect.
  ///
  /// # Safety
  /// Causes undefined behavior if `is_null()` is `true`.
  template <typename Rep, typename Period>
  std::expected<void, TrySendError<T>>
  try_send_for(T item,
               const std::chrono::duration<Rep, Period> &timeout) const {
    assert(this->channel != nullptr);
    return this->channel->try_send_for(std::move(item), timeout);
  }

  /// Send an item on the channel with a deadline.
  ///
  /// Blocks until the channel is not full, the deadline is met, or all
  /// receivers disconnect.
  ///
  /// # Safety
  /// Causes undefined behavior if `is_null()` is `true`.
  template <typename Clock, typename Duration>
  std::expected<void, TrySendError<T>> try_send_until(
      T item, const std::chrono::time_point<Clock, Duration> &deadline) const {
    assert(this->channel != nullptr);
    return this->channel->try_send_until(std::move(item), deadline);
  }

  /// Number of items in the channel.
  ///
  /// Since the channel's size could change at any moment, it should not be used
  /// to determine if a send operation will block/fail.
  ///
  /// # Safety
  /// Causes undefined behavior if `is_null()` is `true`.
  std::size_t channel_size() const {
    assert(this->channel != nullptr);
    return this->channel->size.load(std::memory_order::relaxed);
  }

  /// Number of items the channel has allocated space for.
  ///
  /// # Safety
  /// Causes undefined behavior if `is_null()` is `true`.
  std::size_t channel_capacity() const {
    assert(this->channel != nullptr);
    return this->channel->capacity;
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
} // namespace chan::spmc::bounded

#endif
