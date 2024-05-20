#ifndef _CHAN_MPMC_PACKET_H
#define _CHAN_MPMC_PACKET_H

#include <atomic>

namespace chan::mpmc {
template <typename T> struct Packet {
  T item;
  std::atomic_bool read_ready;
  std::atomic_bool write_ready;
};
} // namespace chan::mpmc

#endif
