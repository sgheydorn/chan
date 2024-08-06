#ifndef _CHAN_SPSC_UNBOUNDED_ITEM_CHUNK_H
#define _CHAN_SPSC_UNBOUNDED_ITEM_CHUNK_H

#include <cstddef>

namespace chan::spsc::unbounded {
/// Chunk of items and a pointer to the next chunk.
///
/// Aside from custom allocators, there is no reason to work with this class
/// directly.
template <typename T, std::size_t CHUNK_SIZE> struct ItemChunk {
  ItemChunk *next;
  T items[CHUNK_SIZE];
};
} // namespace chan::spsc::unbounded

#endif
