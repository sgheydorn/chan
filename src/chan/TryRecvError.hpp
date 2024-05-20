#ifndef _CHAN_TRY_RECV_ERROR_H
#define _CHAN_TRY_RECV_ERROR_H

#include "TryRecvErrorKind.hpp"

namespace chan {
struct TryRecvError {
  TryRecvErrorKind kind;

  bool is_empty() const { return this->kind == TryRecvErrorKind::Empty; }

  bool is_disconnected() const {
    return this->kind == TryRecvErrorKind::Disconnected;
  }
};
} // namespace chan

#endif
