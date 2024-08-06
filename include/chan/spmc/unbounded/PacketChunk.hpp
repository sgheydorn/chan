#ifndef _CHAN_SPMC_UNBOUNDED_PACKET_CHUNK_H
#define _CHAN_SPMC_UNBOUNDED_PACKET_CHUNK_H

#include <cstddef>

#include "../Packet.hpp"

namespace chan::spmc::unbounded {
/// Chunk of items and a pointer to the next chunk.
///
/// Aside from custom allocators, there is no reason to work with this class
/// directly.
template <typename T, std::size_t CHUNK_SIZE> struct PacketChunk {
  PacketChunk *next;
  Packet<T> packets[CHUNK_SIZE];
};
} // namespace chan::spmc::unbounded

#endif
