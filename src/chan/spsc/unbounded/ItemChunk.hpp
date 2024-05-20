#pragma once

#include <cstddef>

namespace chan::spsc::unbounded {
template <typename T, std::size_t CHUNK_SIZE> struct ItemChunk {
  ItemChunk *next;
  T items[CHUNK_SIZE];
};
} // namespace chan::spsc::unbounded
