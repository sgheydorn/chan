#ifndef _CHAN_MPSC_PACKET_H
#define _CHAN_MPSC_PACKET_H

#include <atomic>

namespace chan::mpsc {
template <typename T> struct Packet {
  T item;
  std::atomic_bool ready;
};
} // namespace chan::mpsc

#endif
