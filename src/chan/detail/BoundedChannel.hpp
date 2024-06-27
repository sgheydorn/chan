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
    if (static_cast<Self *>(this)->recv_done()) {
      return std::unexpected(SendError{std::move(item)});
    }
    static_cast<Self *>(this)->send_ready.acquire();
    if (static_cast<Self *>(this)->recv_done()) {
      return std::unexpected(SendError{std::move(item)});
    }
    static_cast<Self *>(this)->do_send(std::move(item));
    static_cast<Self *>(this)->size.fetch_add(1, std::memory_order::relaxed);
    static_cast<Self *>(this)->recv_ready.release();
    return {};
  }

  std::expected<void, TrySendError<T>> try_send(T item) {
    return this->try_send_impl(std::move(item), [&] {
      return static_cast<Self *>(this)->send_ready.try_acquire();
    });
  }

  template <typename Rep, typename Period>
  std::expected<void, TrySendError<T>>
  try_send_for(T item, const std::chrono::duration<Rep, Period> &timeout) {
    return this->try_send_impl(std::move(item), [&] {
      return static_cast<Self *>(this)->send_ready.try_acquire_for(timeout);
    });
  }

  template <typename Clock, typename Duration>
  std::expected<void, TrySendError<T>>
  try_send_until(T item,
                 const std::chrono::time_point<Clock, Duration> &deadline) {
    return this->try_send_impl(std::move(item), [&] {
      return static_cast<Self *>(this)->send_ready.try_acquire_until(deadline);
    });
  }

  std::expected<T, RecvError> recv() {
    if (!static_cast<Self *>(this)->send_done()) {
      static_cast<Self *>(this)->recv_ready.acquire();
      if (this->decrement_size()) {
        return std::unexpected(RecvError{});
      }
      auto item = static_cast<Self *>(this)->do_recv();
      static_cast<Self *>(this)->send_ready.release();
      return item;
    } else {
      if (this->decrement_size()) {
        return std::unexpected(RecvError{});
      }
      return static_cast<Self *>(this)->do_recv();
    }
  }

  std::expected<T, TryRecvError> try_recv() {
    return this->try_recv_impl(
        [&] { return static_cast<Self *>(this)->recv_ready.try_acquire(); });
  }

  template <typename Rep, typename Period>
  std::expected<T, TryRecvError>
  try_recv_for(const std::chrono::duration<Rep, Period> &timeout) {
    return this->try_recv_impl([&] {
      return static_cast<Self *>(this)->recv_ready.try_acquire_for(timeout);
    });
  }

  template <typename Clock, typename Duration>
  std::expected<T, TryRecvError>
  try_recv_until(const std::chrono::time_point<Clock, Duration> &deadline) {
    return this->try_recv_impl([&] {
      return static_cast<Self *>(this)->recv_ready.try_acquire_until(deadline);
    });
  }

private:
  template <typename F>
  std::expected<void, TrySendError<T>> try_send_impl(T item, const F &acquire) {
    if (static_cast<Self *>(this)->recv_done()) {
      return std::unexpected(
          TrySendError{TrySendErrorKind::Disconnected, std::move(item)});
    }
    if (!acquire()) {
      return std::unexpected(
          TrySendError{TrySendErrorKind::Full, std::move(item)});
    }
    if (static_cast<Self *>(this)->recv_done()) {
      return std::unexpected(
          TrySendError{TrySendErrorKind::Disconnected, std::move(item)});
    }
    static_cast<Self *>(this)->do_send(std::move(item));
    static_cast<Self *>(this)->size.fetch_add(1, std::memory_order::relaxed);
    static_cast<Self *>(this)->recv_ready.release();
    return {};
  }

  template <typename F>
  std::expected<T, TryRecvError> try_recv_impl(const F &acquire) {
    if (!static_cast<Self *>(this)->send_done()) {
      if (!acquire()) {
        return std::unexpected(TryRecvError{TryRecvErrorKind::Empty});
      }
      if (this->decrement_size()) {
        return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
      }
      auto item = static_cast<Self *>(this)->do_recv();
      static_cast<Self *>(this)->send_ready.release();
      return item;
    } else {
      if (this->decrement_size()) {
        return std::unexpected(TryRecvError{TryRecvErrorKind::Disconnected});
      }
      return static_cast<Self *>(this)->do_recv();
    }
  }

  bool decrement_size() {
    std::size_t size;
    do {
      size = static_cast<Self *>(this)->size.load(std::memory_order::relaxed);
    } while (size != 0 &&
             !static_cast<Self *>(this)->size.compare_exchange_weak(
                 size, size - 1, std::memory_order::relaxed));
    return size == 0;
  }
};
} // namespace chan::detail

#endif
