#ifndef _CHAN_TRY_RECV_ERROR_HPP
#define _CHAN_TRY_RECV_ERROR_HPP

#include "TryRecvErrorKind.hpp"

namespace chan {
/// Error for `try_recv`, `try_recv_for` and `try_recv_until` operations.
struct TryRecvError {
  /// Reason the operation failed.
  TryRecvErrorKind kind;

  /// Check if `kind` is `TryRecvErrorKind::Empty`.
  bool is_empty() const { return this->kind == TryRecvErrorKind::Empty; }

  /// Check if `kind` is `TryRecvErrorKind::Disconnected`.
  bool is_disconnected() const {
    return this->kind == TryRecvErrorKind::Disconnected;
  }
};
} // namespace chan

#endif
