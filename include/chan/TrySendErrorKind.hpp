#ifndef _CHAN_TRY_SEND_ERROR_KIND_HPP
#define _CHAN_TRY_SEND_ERROR_KIND_HPP

namespace chan {
enum class TrySendErrorKind {
  /// Channel currently has no space to send another item.
  ///
  /// For unbuffered channels, this means there are no pending receive
  /// operations.
  ///
  /// This is a transient error.
  Full,

  /// There are no remaining receivers.
  ///
  /// This is a permanent error.
  Disconnected,
};
}

#endif
