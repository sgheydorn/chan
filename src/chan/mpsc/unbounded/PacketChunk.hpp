#pragma once

#include <cstddef>

#include "../Packet.hpp"

namespace chan::mpsc::unbounded {
template <typename T, std::size_t CHUNK_SIZE> struct PacketChunk {
  PacketChunk *next;
  Packet<T> packets[CHUNK_SIZE];
};
} // namespace chan::mpsc::unbounded
