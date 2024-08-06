#ifndef _CHAN_SPMC_PACKET_H
#define _CHAN_SPMC_PACKET_H

#include <atomic>

namespace chan::spmc {
/// Item with a synchronization flag.
///
/// Aside from custom allocators, there is no reason to work with this class
/// directly.
template <typename T> struct Packet {
  T item;
  std::atomic_bool write_ready;
};
} // namespace chan::spmc

#endif
