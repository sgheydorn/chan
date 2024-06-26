#ifndef _CHAN_MPSC_UNBOUNDED_PACKET_CHUNK_H
#define _CHAN_MPSC_UNBOUNDED_PACKET_CHUNK_H

#include <cstddef>

#include "../Packet.hpp"

namespace chan::mpsc::unbounded {
/// Chunk of items and a pointer to the next chunk.
///
/// Aside from custom allocators, there is no reason to work with this class
/// directly.
template <typename T, std::size_t CHUNK_SIZE> struct PacketChunk {
  PacketChunk *next;
  Packet<T> packets[CHUNK_SIZE];
};
} // namespace chan::mpsc::unbounded

#endif
