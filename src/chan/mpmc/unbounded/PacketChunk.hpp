#ifndef _CHAN_MPMC_UNBOUNDED_PACKET_CHUNK_H
#define _CHAN_MPMC_UNBOUNDED_PACKET_CHUNK_H

#include <cstddef>

#include "../Packet.hpp"

namespace chan::mpmc::unbounded {
template <typename T, std::size_t CHUNK_SIZE> struct PacketChunk {
  PacketChunk *next;
  Packet<T> packets[CHUNK_SIZE];
};
} // namespace chan::mpmc::unbounded

#endif
