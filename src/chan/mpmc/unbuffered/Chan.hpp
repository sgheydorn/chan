#ifndef _CHAN_MPMC_UNBUFFERED_CHANNEL_H
#define _CHAN_MPMC_UNBUFFERED_CHANNEL_H

#include <atomic>
#include <condition_variable>
#include <deque>
#include <expected>
#include <optional>

#include "../../RecvError.hpp"
#include "../../SendError.hpp"
#include "../../TryRecvError.hpp"
#include "../../TrySendError.hpp"

namespace chan::mpmc::unbuffered {
template <typename T, typename A> class Chan {
  template <typename, typename, typename> friend class Sender;
  template <typename, typename, typename> friend class Receiver;

  std::deque<std::optional<T> *, A> send_packets;
  std::deque<std::optional<T> *, A> recv_packets;
  bool send_done;
  bool recv_done;
  std::mutex packet_mutex;
  std::condition_variable send_ready;
  std::condition_variable recv_ready;

  std::atomic_size_t sender_count;
  std::atomic_size_t receiver_count;
  std::atomic_bool disconnected;

public:
  Chan(A allocator)
      : send_packets(allocator), recv_packets(std::move(allocator)),
        send_done(false), recv_done(false), sender_count(1), receiver_count(1),
        disconnected(false) {}

private:
  std::expected<void, SendError<T>> send(T item) {
    std::unique_lock lock(this->packet_mutex);
    if (!this->recv_packets.empty()) {
      this->recv_packets.front()->emplace(std::move(item));
      this->recv_packets.pop_front();
      lock.unlock();
      this->recv_ready.notify_all();
    } else {
      std::optional<T> packet(std::move(item));
      this->send_packets.push_back(&packet);
      this->send_ready.wait(
          lock, [this, &packet] { return !packet || this->recv_done; });
      lock.unlock();
      if (packet) {
        return std::unexpected(SendError{std::move(*packet)});
      }
    }
    return {};
  }

  std::expected<void, TrySendError<T>> try_send(T item) {
    std::unique_lock lock(this->packet_mutex);
    if (this->recv_done) {
      return std::unexpected(
          TrySendError{TrySendErrorKind::Disconnected, std::move(item)});
    }
    if (this->recv_packets.empty()) {
      return std::unexpected(
          TrySendError{TrySendErrorKind::Full, std::move(item)});
    }
    this->recv_packets.front()->emplace(std::move(item));
    this->recv_packets.pop_front();
    lock.unlock();
    this->recv_ready.notify_all();
    return {};
  }

  template <typename Rep, typename Period>
  std::expected<void, TrySendError<T>>
  try_send_for(T item, const std::chrono::duration<Rep, Period> &timeout) {
    std::unique_lock lock(this->packet_mutex);
    if (!this->recv_packets.empty()) {
      this->recv_packets.front()->emplace(std::move(item));
      this->recv_packets.pop_front();
      lock.unlock();
      this->recv_ready.notify_all();
    } else {
      std::optional<T> packet(std::move(item));
      auto entry = this->send_packets.insert(this->send_packets.end(), &packet);
      if (!this->send_ready.wait_for(lock, timeout, [this, &packet] {
            return !packet || this->recv_done;
          })) {
        this->send_packets.erase(entry);
        return std::unexpected(
            TrySendError{TrySendErrorKind::Full, std::move(*packet)});
      }
      lock.unlock();
      if (packet) {
        return std::unexpected(
            TrySendError{TrySendErrorKind::Disconnected, std::move(*packet)});
      }
    }
    return {};
  }

  template <typename Clock, typename Duration>
  std::expected<void, TrySendError<T>>
  try_send_until(T item,
                 const std::chrono::time_point<Clock, Duration> &deadline) {
    std::unique_lock lock(this->packet_mutex);
    if (!this->recv_packets.empty()) {
      this->recv_packets.front()->emplace(std::move(item));
      this->recv_packets.pop_front();
      lock.unlock();
      this->recv_ready.notify_all();
    } else {
      std::optional<T> packet(std::move(item));
      auto entry = this->send_packets.insert(this->send_packets.end(), &packet);
      if (!this->send_ready.wait_until(lock, deadline, [this, &packet] {
            return !packet || this->recv_done;
          })) {
        this->send_packets.erase(entry);
        return std::unexpected(
            TrySendError{TrySendErrorKind::Full, std::move(*packet)});
      }
      lock.unlock();
      if (packet) {
        return std::unexpected(
            TrySendError{TrySendErrorKind::Disconnected, std::move(*packet)});
      }
    }
    return {};
  }

  std::expected<T, RecvError> recv() {
    std::unique_lock lock(this->packet_mutex);
    if (!this->send_packets.empty()) {
      auto item = std::move(**this->send_packets.front());
      this->send_packets.front()->reset();
      this->send_packets.pop_front();
      lock.unlock();
      this->send_ready.notify_all();
      return item;
    } else {
      std::optional<T> packet;
      this->recv_packets.push_back(&packet);
      this->recv_ready.wait(
          lock, [this, &packet] { return packet || this->send_done; });
      lock.unlock();
      if (!packet) {
        return std::unexpected(RecvError{});
      }
      return std::move(*packet);
    }
  }

  std::expected<T, TryRecvError> try_recv() {
    std::unique_lock lock(this->packet_mutex);
    if (this->send_done) {
      return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
    }
    if (this->send_packets.empty()) {
      return std::unexpected(TryRecvError{TryRecvErrorKind::Empty});
    }
    auto item = std::move(**this->send_packets.front());
    this->send_packets.front()->reset();
    this->send_packets.pop_front();
    lock.unlock();
    this->send_ready.notify_all();
    return item;
  }

  template <typename Rep, typename Period>
  std::expected<T, TryRecvError>
  try_recv_for(const std::chrono::duration<Rep, Period> &timeout) {
    std::unique_lock lock(this->packet_mutex);
    if (!this->send_packets.empty()) {
      auto item = std::move(**this->send_packets.front());
      this->send_packets.front()->reset();
      this->send_packets.pop_front();
      lock.unlock();
      this->send_ready.notify_all();
      return item;
    } else {
      std::optional<T> packet;
      auto entry = this->recv_packets.insert(this->recv_packets.end(), &packet);
      if (!this->recv_ready.wait_for(lock, timeout, [this, &packet] {
            return packet || this->send_done;
          })) {
        this->recv_packets.erase(entry);
        return std::unexpected(TryRecvError{TryRecvErrorKind::Empty});
      }
      lock.unlock();
      if (!packet) {
        return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
      }
      return std::move(*packet);
    }
  }

  template <typename Clock, typename Duration>
  std::expected<T, TryRecvError>
  try_recv_until(const std::chrono::time_point<Clock, Duration> &deadline) {
    std::unique_lock lock(this->packet_mutex);
    if (!this->send_packets.empty()) {
      auto item = std::move(**this->send_packets.front());
      this->send_packets.front()->reset();
      this->send_packets.pop_front();
      lock.unlock();
      this->send_ready.notify_all();
      return item;
    } else {
      std::optional<T> packet;
      auto entry = this->recv_packets.insert(this->recv_packets.end(), &packet);
      if (!this->recv_ready.wait_until(lock, deadline, [this, &packet] {
            return packet || this->send_done;
          })) {
        this->recv_packets.erase(entry);
        return std::unexpected(TryRecvError{TryRecvErrorKind::Empty});
      }
      lock.unlock();
      if (!packet) {
        return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
      }
      return std::move(*packet);
    }
  }

  void acquire_sender() {
    this->sender_count.fetch_add(1, std::memory_order::relaxed);
  }

  void acquire_receiver() {
    this->receiver_count.fetch_add(1, std::memory_order::relaxed);
  }

  bool release_sender() {
    if (this->sender_count.fetch_sub(1, std::memory_order::acq_rel) != 1) {
      return false;
    }
    {
      std::lock_guard _lock(this->packet_mutex);
      this->send_done = true;
    }
    this->recv_ready.notify_all();
    return this->disconnected.exchange(true, std::memory_order::relaxed);
  }

  bool release_receiver() {
    if (this->receiver_count.fetch_sub(1, std::memory_order::acq_rel) != 1) {
      return false;
    }
    {
      std::lock_guard _lock(this->packet_mutex);
      this->recv_done = true;
    }
    this->send_ready.notify_all();
    return this->disconnected.exchange(true, std::memory_order::relaxed);
  }
};
} // namespace chan::mpmc::unbuffered

#endif
