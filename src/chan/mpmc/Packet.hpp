#pragma once

#include <atomic>

namespace chan::mpmc {
template <typename T> struct Packet {
  T item;
  std::atomic_bool read_ready;
  std::atomic_bool write_ready;
};
} // namespace chan::mpmc
