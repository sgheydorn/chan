#ifndef _CHAN_MPSC_PACKET_H
#define _CHAN_MPSC_PACKET_H

#include <atomic>

namespace chan::mpsc {
/// Item with a synchronization flag.
///
/// Aside from custom allocators, there is no reason to work with this class
/// directly.
template <typename T> struct Packet {
  T item;
  std::atomic_bool ready;
};
} // namespace chan::mpsc

#endif
