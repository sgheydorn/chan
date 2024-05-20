#ifndef _CHAN_TRY_SEND_ERROR_H
#define _CHAN_TRY_SEND_ERROR_H

#include "TrySendErrorKind.hpp"

namespace chan {
template <typename T> struct TrySendError {
  TrySendErrorKind kind;
  T item;

  bool is_full() const { return this->kind == TrySendErrorKind::Full; }

  bool is_disconnected() const {
    return this->kind == TrySendErrorKind::Disconnected;
  }
};
} // namespace chan

#endif
