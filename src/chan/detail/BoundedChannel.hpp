#ifndef _CHAN_DETAIL_BOUNDED_CHANNEL
#define _CHAN_DETAIL_BOUNDED_CHANNEL

#include <expected>

#include "../RecvError.hpp"
#include "../SendError.hpp"
#include "../TryRecvError.hpp"
#include "../TrySendError.hpp"

namespace chan::detail {
template <typename Self, typename T> struct BoundedChannel {
  std::expected<void, SendError<T>> send(T item) {
    static_cast<Self *>(this)->send_ready.acquire();
    auto ok = static_cast<Self *>(this)->send_impl(item);
    static_cast<Self *>(this)->recv_ready.release();

    if (ok) {
      return {};
    } else {
      return std::unexpected(SendError{std::move(item)});
    }
  }

  std::expected<void, TrySendError<T>> try_send(T item) {
    if (!static_cast<Self *>(this)->send_ready.try_acquire()) {
      return std::unexpected(
          TrySendError{TrySendErrorKind::Full, std::move(item)});
    }
    auto ok = static_cast<Self *>(this)->send_impl(item);
    static_cast<Self *>(this)->recv_ready.release();

    if (ok) {
      return {};
    } else {
      return std::unexpected(
          TrySendError{TrySendErrorKind::Disconnected, std::move(item)});
    }
  }

  template <typename Rep, typename Period>
  std::expected<void, TrySendError<T>>
  try_send_for(T item, const std::chrono::duration<Rep, Period> &timeout) {
    if (!static_cast<Self *>(this)->send_ready.try_acquire_for(timeout)) {
      return std::unexpected(
          TrySendError{TrySendErrorKind::Full, std::move(item)});
    }
    auto ok = static_cast<Self *>(this)->send_impl(item);
    static_cast<Self *>(this)->recv_ready.release();

    if (ok) {
      return {};
    } else {
      return std::unexpected(
          TrySendError{TrySendErrorKind::Disconnected, std::move(item)});
    }
  }

  template <typename Clock, typename Duration>
  std::expected<void, TrySendError<T>>
  try_send_until(T item,
                 const std::chrono::time_point<Clock, Duration> &deadline) {
    if (!static_cast<Self *>(this)->send_ready.try_acquire_until(deadline)) {
      return std::unexpected(
          TrySendError{TrySendErrorKind::Full, std::move(item)});
    }
    auto ok = static_cast<Self *>(this)->send_impl(item);
    static_cast<Self *>(this)->recv_ready.release();

    if (ok) {
      return {};
    } else {
      return std::unexpected(
          TrySendError{TrySendErrorKind::Disconnected, std::move(item)});
    }
  }

  std::expected<T, RecvError> recv() {
    static_cast<Self *>(this)->recv_ready.acquire();
    auto item = static_cast<Self *>(this)->recv_impl();
    static_cast<Self *>(this)->send_ready.release();

    if (item) {
      return std::move(*item);
    } else {
      return std::unexpected(RecvError{});
    }
  }

  std::expected<T, TryRecvError> try_recv() {
    if (!static_cast<Self *>(this)->recv_ready.try_acquire()) {
      return std::unexpected(TryRecvError{TryRecvErrorKind::Empty});
    }
    auto item = static_cast<Self *>(this)->recv_impl();
    static_cast<Self *>(this)->send_ready.release();

    if (item) {
      return std::move(*item);
    } else {
      return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
    }
  }

  template <typename Rep, typename Period>
  std::expected<T, TryRecvError>
  try_recv_for(const std::chrono::duration<Rep, Period> &timeout) {
    if (!static_cast<Self *>(this)->recv_ready.try_acquire_for(timeout)) {
      return std::unexpected(TryRecvError{TryRecvErrorKind::Empty});
    }
    auto item = static_cast<Self *>(this)->recv_impl();
    static_cast<Self *>(this)->send_ready.release();

    if (item) {
      return std::move(*item);
    } else {
      return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
    }
  }

  template <typename Clock, typename Duration>
  std::expected<T, TryRecvError>
  try_recv_until(const std::chrono::time_point<Clock, Duration> &deadline) {
    if (!static_cast<Self *>(this)->recv_ready.try_acquire_until(deadline)) {
      return std::unexpected(TryRecvError{TryRecvErrorKind::Empty});
    }
    auto item = static_cast<Self *>(this)->recv_impl();
    static_cast<Self *>(this)->send_ready.release();

    if (item) {
      return std::move(*item);
    } else {
      return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
    }
  }
};
} // namespace chan::detail

#endif
