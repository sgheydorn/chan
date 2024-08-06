#ifndef _CHAN_TRY_RECV_ERROR_KIND_H
#define _CHAN_TRY_RECV_ERROR_KIND_H

namespace chan {
enum class TryRecvErrorKind {
  /// Channel currently has no items to receive.
  ///
  /// For unbuffered channels, this means there are no pending send operations.
  ///
  /// This is a transient error.
  Empty,

  /// Channel has no items to receive and there are no remaining senders.
  ///
  /// This is a permanent error.
  Disconnected,
};
}

#endif
