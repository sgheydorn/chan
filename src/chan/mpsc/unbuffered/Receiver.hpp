#pragma once

#include <memory>

#include "../../RecvIter.hpp"
#include "Channel.hpp"

namespace chan::mpsc::unbuffered {
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
      return std::unexpected(RecvError());
    }
    auto result = this->channel->recv();
    if (!result) {
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
      std::allocator_traits<A>::destroy(this->allocator, this->channel);
      std::allocator_traits<A>::deallocate(this->allocator, this->channel, 1);
    }
  }

public:
  chan::RecvIter<Receiver> begin() { return chan::RecvIter<Receiver>(*this); }

  std::default_sentinel_t end() { return {}; }
};
} // namespace chan::mpsc::unbuffered
