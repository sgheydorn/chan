#ifndef _CHAN_SPSC_UNBUFFERED_CHANNEL_H
#define _CHAN_SPSC_UNBUFFERED_CHANNEL_H

#include <atomic>
#include <expected>
#include <optional>

#include "../../RecvError.hpp"
#include "../../detail/SemaphoreType.hpp"
#include "../../detail/UnbufferedChannel.hpp"

namespace chan::spsc::unbuffered {
template <typename T> class Channel : detail::UnbufferedChannel<Channel<T>, T> {
  friend struct detail::UnbufferedChannel<Channel, T>;
  template <typename, typename> friend class Sender;
  template <typename, typename> friend class Receiver;

  std::atomic<std::optional<T> *> packet;
  detail::SemaphoreType send_ready;
  detail::SemaphoreType recv_ready;
  std::atomic_bool disconnected;

public:
  Channel()
      : packet(nullptr), send_ready(0), recv_ready(0), disconnected(false) {}

private:
  bool send_impl(T &item) {
    auto packet = this->packet.exchange(nullptr, std::memory_order::relaxed);
    if (packet) {
      *packet = std::move(item);
      return true;
    } else {
      return false;
    }
  }

  std::expected<T, RecvError> recv() {
    std::optional<T> packet;
    this->packet.store(&packet, std::memory_order::relaxed);
    this->send_ready.release();
    this->recv_ready.acquire();
    if (!packet) {
      return std::unexpected(RecvError{});
    }
    return std::move(*packet);
  }

  bool release_sender() {
    this->recv_ready.release();
    return this->disconnected.exchange(true, std::memory_order::relaxed);
  }

  bool release_receiver() {
    this->send_ready.release();
    return this->disconnected.exchange(true, std::memory_order::relaxed);
  }
};
} // namespace chan::spsc::unbuffered

#endif
