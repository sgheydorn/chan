#ifndef _CHAN_SEND_ERROR_HPP
#define _CHAN_SEND_ERROR_HPP

namespace chan {
/// Error for the `send` operation. Occurs when there are no remaining
/// receivers.
template <typename T> struct SendError {
  /// Item that failed to send. Allows the caller to recover the item if needed.
  T item;
};
} // namespace chan

#endif
