#ifndef _CHAN_SPSC_BOUNDED_RECEIVER_H
#define _CHAN_SPSC_BOUNDED_RECEIVER_H

#include <memory>

#include "../../RecvIter.hpp"
#include "Channel.hpp"

namespace chan::spsc::bounded {
template <typename T, typename A1 = std::allocator<T>,
          typename A2 = std::allocator<Channel<T, A1>>>
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

  Receiver(const Receiver &) = delete;
  Receiver &operator=(const Receiver &) = delete;

  std::expected<T, RecvError> recv() {
    if (!this->channel) {
      return std::unexpected(RecvError());
    }
    auto result = this->channel->recv();
    if (!result) {
      this->disconnect();
    }
    return result;
  }

  std::expected<T, TryRecvError> try_recv() {
    if (!this->channel) {
      return std::unexpected(TryRecvError(TryRecvErrorKind::Disconnected));
    }
    auto result = this->channel->try_recv();
    if (!result && result.error().is_disconnected()) {
      this->disconnect();
    }
    return result;
  }

  template <typename Rep, typename Period>
  std::expected<T, TryRecvError>
  try_recv_for(const std::chrono::duration<Rep, Period> &timeout) {
    if (!this->channel) {
      return std::unexpected(TryRecvError(TryRecvErrorKind::Disconnected));
    }
    auto result = this->channel->try_recv_for(timeout);
    if (!result && result.error().is_disconnected()) {
      this->disconnect();
    }
    return result;
  }

  template <typename Clock, typename Duration>
  std::expected<T, TryRecvError>
  try_recv_until(const std::chrono::duration<Clock, Duration> &deadline) {
    if (!this->channel) {
      return std::unexpected(TryRecvError(TryRecvErrorKind::Disconnected));
    }
    auto result = this->channel->try_recv_until(deadline);
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
  void release() {
    if (this->channel && this->channel->release_receiver()) {
      std::allocator_traits<A2>::destroy(this->allocator, this->channel);
      std::allocator_traits<A2>::deallocate(this->allocator, this->channel, 1);
    }
  }

public:
  chan::RecvIter<Receiver> begin() { return chan::RecvIter<Receiver>(*this); }

  std::default_sentinel_t end() { return {}; }
};
} // namespace chan::spsc::bounded

#endif
