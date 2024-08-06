#ifndef _CHAN_SPSC_UNBUFFERED_CHANNEL_H
#define _CHAN_SPSC_UNBUFFERED_CHANNEL_H

#include <atomic>
#include <condition_variable>
#include <expected>
#include <optional>

#include "../../detail/UnbufferedChannel.hpp"

namespace chan::spsc::unbuffered {
/// Channel implementation.
///
/// Aside from custom allocators, there is no reason to work with this class
/// directly.
template <typename T> class Chan : detail::UnbufferedChannel<Chan<T>, T> {
  friend struct detail::UnbufferedChannel<Chan, T>;
  template <typename, typename> friend class Sender;
  template <typename, typename> friend class Receiver;

  std::optional<T> *send_packet;
  std::optional<T> *recv_packet;
  bool send_done;
  bool recv_done;
  std::mutex packet_mutex;
  std::condition_variable send_ready;
  std::condition_variable recv_ready;

  std::atomic_bool disconnected;

public:
  /// Create a channel that assumes a single `Sender` and single `Receiver`.
  ///
  /// This constructor should not be called directly. Instead, call the
  /// `channel` function.
  Chan()
      : send_packet(nullptr), recv_packet(nullptr), send_done(false),
        recv_done(false), disconnected(false) {}

private:
  bool has_send_packet() const { return this->send_packet != nullptr; }

  T take_send_packet() {
    auto item = std::move(**this->send_packet);
    this->send_packet->reset();
    this->send_packet = nullptr;
    return item;
  }

  void register_send_packet(std::optional<T> *packet) {
    this->send_packet = packet;
  }

  void unregister_send_packet(std::optional<T> *) {
    this->send_packet = nullptr;
  }

  bool has_recv_packet() const { return this->recv_packet != nullptr; }

  void set_recv_packet(T item) {
    this->recv_packet->emplace(std::move(item));
    this->recv_packet = nullptr;
  }

  void register_recv_packet(std::optional<T> *packet) {
    this->recv_packet = packet;
  }

  void unregister_recv_packet(std::optional<T> *) {
    this->recv_packet = nullptr;
  }

  bool release_sender() {
    {
      std::lock_guard _lock(this->packet_mutex);
      this->send_done = true;
    }
    this->recv_ready.notify_all();
    return this->disconnected.exchange(true, std::memory_order::acq_rel);
  }

  bool release_receiver() {
    {
      std::lock_guard _lock(this->packet_mutex);
      this->recv_done = true;
    }
    this->send_ready.notify_all();
    return this->disconnected.exchange(true, std::memory_order::acq_rel);
  }
};
} // namespace chan::spsc::unbuffered

#endif
