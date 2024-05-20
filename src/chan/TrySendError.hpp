#pragma once

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
