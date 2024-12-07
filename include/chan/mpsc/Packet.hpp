#ifndef _CHAN_MPSC_PACKET_HPP
#define _CHAN_MPSC_PACKET_HPP

#include <atomic>

namespace chan::mpsc {
/// Item with a synchronization flag.
///
/// Aside from custom allocators, there is no reason to work with this class
/// directly.
template <typename T> struct Packet {
  T item;
  std::atomic_bool read_ready;
};
} // namespace chan::mpsc

#endif
