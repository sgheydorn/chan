#ifndef _CHAN_TRY_SEND_ERROR_H
#define _CHAN_TRY_SEND_ERROR_H

#include "TrySendErrorKind.hpp"

namespace chan {
/// Error for `try_send`, `try_send_for` and `try_send_until` operations.
template <typename T> struct TrySendError {
  /// Reason the operation failed.
  TrySendErrorKind kind;

  /// Item that failed to send. Allows the caller to recover the item if needed.
  T item;

  /// Check if `kind` is `TrySendErrorKind::Full`.
  bool is_full() const { return this->kind == TrySendErrorKind::Full; }

  /// Check if `kind` is `TrySendErrorKind::Disconnected`.
  bool is_disconnected() const {
    return this->kind == TrySendErrorKind::Disconnected;
  }
};
} // namespace chan

#endif
