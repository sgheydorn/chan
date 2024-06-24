#ifndef _CHAN_MPSC_BOUNDED_SENDER_H
#define _CHAN_MPSC_BOUNDED_SENDER_H

#include <memory>

#include "../../SendIter.hpp"
#include "Channel.hpp"

namespace chan::mpsc::bounded {
template <typename T, typename A1 = std::allocator<Packet<T>>,
          typename A2 = std::allocator<Channel<T, A1>>>
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

  std::expected<void, TrySendError<T>> try_send(T item) const {
    return this->channel->try_send(std::move(item));
  }

  template <typename Rep, typename Period>
  std::expected<void, TrySendError<T>>
  try_send_for(T item,
               const std::chrono::duration<Rep, Period> &timeout) const {
    return this->channel->try_send_for(std::move(item), timeout);
  }

  template <typename Clock, typename Duration>
  std::expected<void, TrySendError<T>> try_send_until(
      T item, const std::chrono::time_point<Clock, Duration> &deadline) const {
    return this->channel->try_send_until(std::move(item), deadline);
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
} // namespace chan::mpsc::bounded

#endif
