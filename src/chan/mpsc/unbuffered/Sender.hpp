#pragma once

#include <memory>

#include "../../SendIter.hpp"
#include "Channel.hpp"

namespace chan::mpsc::unbuffered {
template <typename T, typename A = std::allocator<Channel<T>>> class Sender {
public:
  using Item = T;

private:
  std::allocator_traits<A>::pointer channel;
  A allocator;

public:
  Sender(std::allocator_traits<A>::pointer channel, A allocator)
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

  std::expected<void, SendError<T>> send(T item) {
    if (!this->channel) {
      return std::unexpected(SendError(std::move(item)));
    }
    auto result = this->channel->send(std::move(item));
    if (!result) {
      this->disconnect();
    }
    return result;
  }

  std::expected<void, TrySendError<T>> try_send(T item) {
    if (!this->channel) {
      return std::unexpected(
          TrySendError(TrySendErrorKind::Disconnected, std::move(item)));
    }
    auto result = this->channel->try_send(std::move(item));
    if (!result && result.error().is_disconnected()) {
      this->disconnect();
    }
    return result;
  }

  template <typename Rep, typename Period>
  std::expected<void, TrySendError<T>>
  try_send_for(T item, const std::chrono::duration<Rep, Period> &timeout) {
    if (!this->channel) {
      return std::unexpected(
          TrySendError(TrySendErrorKind::Disconnected, std::move(item)));
    }
    auto result = this->channel->try_send_for(std::move(item), timeout);
    if (!result && result.error().is_disconnected()) {
      this->disconnect();
    }
    return result;
  }

  template <typename Clock, typename Duration>
  std::expected<void, TrySendError<T>>
  try_send_until(T item,
                 const std::chrono::time_point<Clock, Duration> &deadline) {
    if (!this->channel) {
      return std::unexpected(
          TrySendError(TrySendErrorKind::Disconnected, std::move(item)));
    }
    auto result = this->channel->try_send_until(std::move(item), deadline);
    if (!result && result.error().is_disconnected()) {
      this->disconnect();
    }
    return result;
  }

  void disconnect() {
    this->release();
    this->channel = nullptr;
  }

private:
  void acquire() {
    if (this->channel && !this->channel->acquire_sender()) {
      this->channel = nullptr;
    }
  }

  void release() {
    if (this->channel && this->channel->release_sender()) {
      std::allocator_traits<A>::destroy(this->allocator, this->channel);
      std::allocator_traits<A>::deallocate(this->allocator, this->channel, 1);
    }
  }

public:
  chan::SendIter<Sender> begin() { return chan::SendIter(*this); }

  std::default_sentinel_t end() { return {}; }
};
} // namespace chan::mpsc::unbuffered
