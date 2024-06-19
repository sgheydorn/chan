#ifndef _CHAN_SPSC_UNBUFFERED_RECEIVER_H
#define _CHAN_SPSC_UNBUFFERED_RECEIVER_H

#include <memory>

#include "../../RecvIter.hpp"
#include "Channel.hpp"

namespace chan::spsc::unbuffered {
template <typename T, typename A = std::allocator<Channel<T>>> class Receiver {
public:
  using Item = T;

private:
  std::allocator_traits<A>::pointer channel;
  A allocator;

public:
  Receiver(std::allocator_traits<A>::pointer channel, A allocator)
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

  Receiver(const Receiver &) = delete;
  Receiver &operator=(const Receiver &) = delete;

  std::expected<T, RecvError> recv() {
    if (!this->channel) {
      return std::unexpected(RecvError{});
    }
    return this->channel->recv();
  }

  std::expected<T, TryRecvError> try_recv() {
    if (!this->channel) {
      return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
    }
    return this->channel->try_recv();
  }

  template <typename Rep, typename Period>
  std::expected<T, TryRecvError>
  try_recv_for(const std::chrono::duration<Rep, Period> &timeout) {
    if (!this->channel) {
      return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
    }
    return this->channel->try_recv_for(timeout);
  }

  template <typename Clock, typename Duration>
  std::expected<T, TryRecvError>
  try_recv_until(const std::chrono::time_point<Clock, Duration> &deadline) {
    if (!this->channel) {
      return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
    }
    return this->channel->try_recv_until(deadline);
  }

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
  chan::RecvIter<Receiver> begin() { return chan::RecvIter<Receiver>(*this); }

  std::default_sentinel_t end() { return {}; }
};
} // namespace chan::spsc::unbuffered

#endif
