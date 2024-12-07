#ifndef _CHAN_MPMC_PACKET_HPP
#define _CHAN_MPMC_PACKET_HPP

#include <atomic>

namespace chan::mpmc {
/// Item with synchronization flags.
///
/// Aside from custom allocators, there is no reason to work with this class
/// directly.
template <typename T> struct Packet {
  T item;
  std::atomic_bool read_ready;
  std::atomic_bool write_ready;
};
} // namespace chan::mpmc

#endif
