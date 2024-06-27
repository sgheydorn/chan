#ifndef _CHAN_DETAIL_UNBUFFERED_CHANNEL
#define _CHAN_DETAIL_UNBUFFERED_CHANNEL

#include <expected>

#include "../RecvError.hpp"
#include "../SendError.hpp"
#include "../TryRecvError.hpp"
#include "../TrySendError.hpp"

namespace chan::detail {
template <typename Self, typename T> struct UnbufferedChannel {
  std::expected<void, SendError<T>> send(T item) {
    std::unique_lock lock(static_cast<Self *>(this)->packet_mutex);
    if (static_cast<Self *>(this)->has_recv_packet()) {
      static_cast<Self *>(this)->set_recv_packet(std::move(item));
      lock.unlock();
      static_cast<Self *>(this)->recv_ready.notify_all();
    } else {
      std::optional<T> packet(std::move(item));
      static_cast<Self *>(this)->register_send_packet(&packet);
      static_cast<Self *>(this)->send_ready.wait(lock, [this, &packet] {
        return !packet || static_cast<Self *>(this)->recv_done;
      });
      lock.unlock();
      if (packet) {
        return std::unexpected(SendError{std::move(*packet)});
      }
    }
    return {};
  }

  std::expected<void, TrySendError<T>> try_send(T item) {
    {
      std::lock_guard _lock(static_cast<Self *>(this)->packet_mutex);
      if (static_cast<Self *>(this)->recv_done) {
        return std::unexpected(
            TrySendError{TrySendErrorKind::Disconnected, std::move(item)});
      }
      if (!static_cast<Self *>(this)->has_recv_packet()) {
        return std::unexpected(
            TrySendError{TrySendErrorKind::Full, std::move(item)});
      }
      static_cast<Self *>(this)->set_recv_packet(std::move(item));
    }
    static_cast<Self *>(this)->recv_ready.notify_all();
    return {};
  }

  template <typename Rep, typename Period>
  std::expected<void, TrySendError<T>>
  try_send_for(T item, const std::chrono::duration<Rep, Period> &timeout) {
    std::unique_lock lock(static_cast<Self *>(this)->packet_mutex);
    if (static_cast<Self *>(this)->has_recv_packet()) {
      static_cast<Self *>(this)->set_recv_packet(std::move(item));
      lock.unlock();
      static_cast<Self *>(this)->recv_ready.notify_all();
    } else {
      std::optional<T> packet(std::move(item));
      static_cast<Self *>(this)->register_send_packet(&packet);
      if (!static_cast<Self *>(this)->send_ready.wait_for(
              lock, timeout, [this, &packet] {
                return !packet || static_cast<Self *>(this)->recv_done;
              })) {
        static_cast<Self *>(this)->unregister_send_packet(&packet);
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
    std::unique_lock lock(static_cast<Self *>(this)->packet_mutex);
    if (static_cast<Self *>(this)->has_recv_packet()) {
      static_cast<Self *>(this)->set_recv_packet(std::move(item));
      lock.unlock();
      static_cast<Self *>(this)->recv_ready.notify_all();
    } else {
      std::optional<T> packet(std::move(item));
      static_cast<Self *>(this)->register_send_packet(&packet);
      if (!static_cast<Self *>(this)->send_ready.wait_until(
              lock, deadline, [this, &packet] {
                return !packet || static_cast<Self *>(this)->recv_done;
              })) {
        static_cast<Self *>(this)->unregister_send_packet(&packet);
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
    std::unique_lock lock(static_cast<Self *>(this)->packet_mutex);
    if (static_cast<Self *>(this)->has_send_packet()) {
      auto item = static_cast<Self *>(this)->take_send_packet();
      lock.unlock();
      static_cast<Self *>(this)->send_ready.notify_all();
      return item;
    } else {
      std::optional<T> packet;
      static_cast<Self *>(this)->register_recv_packet(&packet);
      static_cast<Self *>(this)->recv_ready.wait(lock, [this, &packet] {
        return packet || static_cast<Self *>(this)->send_done;
      });
      lock.unlock();
      if (!packet) {
        return std::unexpected(RecvError{});
      }
      return std::move(*packet);
    }
  }

  std::expected<T, TryRecvError> try_recv() {
    std::unique_lock lock(static_cast<Self *>(this)->packet_mutex);
    if (static_cast<Self *>(this)->send_done) {
      return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
    }
    if (!static_cast<Self *>(this)->has_send_packet()) {
      return std::unexpected(TryRecvError{TryRecvErrorKind::Empty});
    }
    auto item = static_cast<Self *>(this)->take_send_packet();
    lock.unlock();
    static_cast<Self *>(this)->send_ready.notify_all();
    return item;
  }

  template <typename Rep, typename Period>
  std::expected<T, TryRecvError>
  try_recv_for(const std::chrono::duration<Rep, Period> &timeout) {
    std::unique_lock lock(static_cast<Self *>(this)->packet_mutex);
    if (static_cast<Self *>(this)->has_send_packet()) {
      auto item = static_cast<Self *>(this)->take_send_packet();
      lock.unlock();
      static_cast<Self *>(this)->send_ready.notify_all();
      return item;
    } else {
      std::optional<T> packet;
      static_cast<Self *>(this)->register_recv_packet(&packet);
      if (!static_cast<Self *>(this)->recv_ready.wait_for(
              lock, timeout, [this, &packet] {
                return packet || static_cast<Self *>(this)->send_done;
              })) {
        static_cast<Self *>(this)->unregister_recv_packet(&packet);
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
    std::unique_lock lock(static_cast<Self *>(this)->packet_mutex);
    if (static_cast<Self *>(this)->has_send_packet()) {
      auto item = static_cast<Self *>(this)->take_send_packet();
      lock.unlock();
      static_cast<Self *>(this)->send_ready.notify_all();
      return item;
    } else {
      std::optional<T> packet;
      static_cast<Self *>(this)->register_recv_packet(&packet);
      if (!static_cast<Self *>(this)->recv_ready.wait_until(
              lock, deadline, [this, &packet] {
                return packet || static_cast<Self *>(this)->send_done;
              })) {
        static_cast<Self *>(this)->unregister_recv_packet(&packet);
        return std::unexpected(TryRecvError{TryRecvErrorKind::Empty});
      }
      lock.unlock();
      if (!packet) {
        return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
      }
      return std::move(*packet);
    }
  }

private:
};
} // namespace chan::detail

#endif
