#pragma once

#include <atomic>

namespace chan::mpsc {
template <typename T> struct Packet {
  T item;
  std::atomic_bool ready;
};
} // namespace chan::mpsc
