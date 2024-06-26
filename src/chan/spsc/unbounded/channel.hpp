#ifndef _CHAN_SPSC_UNBOUNDED_CREATE_H
#define _CHAN_SPSC_UNBOUNDED_CREATE_H

#include "../../DEFAULT_CHUNK_SIZE.hpp"
#include "Chan.hpp"
#include "ItemChunk.hpp"
#include "Receiver.hpp"
#include "Sender.hpp"

namespace chan::spsc::unbounded {
template <typename T, std::size_t CHUNK_SIZE = DEFAULT_CHUNK_SIZE,
          typename A1 = std::allocator<ItemChunk<T, CHUNK_SIZE>>,
          typename A2 = std::allocator<Chan<T, CHUNK_SIZE, A1>>>
std::pair<Sender<T, CHUNK_SIZE, A1, A2>, Receiver<T, CHUNK_SIZE, A1, A2>>
channel(A1 chunk_allocator = A1(), A2 channel_allocator = A2()) {
  auto channel = std::allocator_traits<A2>::allocate(channel_allocator, 1);
  std::allocator_traits<A2>::construct(channel_allocator, channel,
                                       std::move(chunk_allocator));
  Sender<T, CHUNK_SIZE, A1, A2> sender(channel, channel_allocator);
  Receiver<T, CHUNK_SIZE, A1, A2> receiver(std::move(channel),
                                           std::move(channel_allocator));
  return {std::move(sender), std::move(receiver)};
}
} // namespace chan::spsc::unbounded

#endif
