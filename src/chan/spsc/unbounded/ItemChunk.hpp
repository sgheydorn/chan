#ifndef _CHAN_SPSC_UNBOUNDED_ITEM_CHUNK_H
#define _CHAN_SPSC_UNBOUNDED_ITEM_CHUNK_H

#include <cstddef>

namespace chan::spsc::unbounded {
template <typename T, std::size_t CHUNK_SIZE> struct ItemChunk {
  ItemChunk *next;
  T items[CHUNK_SIZE];
};
} // namespace chan::spsc::unbounded

#endif
