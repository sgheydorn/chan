#ifndef _CHAN_DETAIL_UNBOUNDED_CHANNEL_H
#define _CHAN_DETAIL_UNBOUNDED_CHANNEL_H

#include <expected>

#include "../RecvError.hpp"
#include "../TryRecvError.hpp"

namespace chan::detail {
template <typename Self, typename T> struct UnboundedChannel {
  std::expected<T, RecvError> recv() {
    if (!static_cast<Self *>(this)->send_done()) {
      static_cast<Self *>(this)->recv_ready.acquire();
      auto item = static_cast<Self *>(this)->recv_impl();
      if (item) {
        return std::move(*item);
      } else {
        return std::unexpected(RecvError{});
      }
    } else {
      auto item = static_cast<Self *>(this)->recv_impl();
      if (item) {
        return std::move(*item);
      } else {
        return std::unexpected(RecvError{});
      }
    }
  }

  std::expected<T, TryRecvError> try_recv() {
    if (!static_cast<Self *>(this)->send_done()) {
      if (!static_cast<Self *>(this)->recv_ready.try_acquire()) {
        return std::unexpected(TryRecvError{TryRecvErrorKind::Empty});
      }
      auto item = static_cast<Self *>(this)->recv_impl();
      if (item) {
        return std::move(*item);
      } else {
        return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
      }
    } else {
      auto item = static_cast<Self *>(this)->recv_impl();
      if (item) {
        return std::move(*item);
      } else {
        return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
      }
    }
  }

  template <typename Rep, typename Period>
  std::expected<T, TryRecvError>
  try_recv_for(const std::chrono::duration<Rep, Period> &timeout) {
    if (!static_cast<Self *>(this)->send_done()) {
      if (!static_cast<Self *>(this)->recv_ready.try_acquire_for(timeout)) {
        return std::unexpected(TryRecvError{TryRecvErrorKind::Empty});
      }
      auto item = static_cast<Self *>(this)->recv_impl();
      if (item) {
        return std::move(*item);
      } else {
        return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
      }
    } else {
      auto item = static_cast<Self *>(this)->recv_impl();
      if (item) {
        return std::move(*item);
      } else {
        return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
      }
    }
  }

  template <typename Clock, typename Duration>
  std::expected<T, TryRecvError>
  try_recv_until(const std::chrono::time_point<Clock, Duration> &deadline) {
    if (!static_cast<Self *>(this)->send_done()) {
      if (!static_cast<Self *>(this)->recv_ready.try_acquire_until(deadline)) {
        return std::unexpected(TryRecvError{TryRecvErrorKind::Empty});
      }
      auto item = static_cast<Self *>(this)->recv_impl();
      if (item) {
        return std::move(*item);
      } else {
        return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
      }
    } else {
      auto item = static_cast<Self *>(this)->recv_impl();
      if (item) {
        return std::move(*item);
      } else {
        return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
      }
    }
  }
};
} // namespace chan::detail

#endif
