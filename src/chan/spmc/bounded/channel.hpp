#ifndef _CHAN_SPMC_BOUNDED_CREATE_H
#define _CHAN_SPMC_BOUNDED_CREATE_H

#include "Chan.hpp"
#include "Receiver.hpp"
#include "Sender.hpp"

namespace chan::spmc::bounded {
/// Create a new channel and get a `Sender` and `Receiver` for it.
///
/// # Parameters
/// `capacity` - Size of the channel's item buffer
/// `buffer_allocator` (optional) - Allocator for the channel's item buffer
/// `channel_allocator` (optional) - Allocator for the channel object
///
/// # Template parameters
/// `T` - Item type of the channel
/// `A1` (optional) - Type of `buffer_allocator` parameter
/// `A2` (optional) - Type of `channel_allocator` parameter
template <typename T, typename A1 = std::allocator<Packet<T>>,
          typename A2 = std::allocator<Chan<T, A1>>>
std::pair<Sender<T, A1, A2>, Receiver<T, A1, A2>>
channel(std::size_t capacity, A1 buffer_allocator = A1(),
        A2 channel_allocator = A2()) {
  auto channel = std::allocator_traits<A2>::allocate(channel_allocator, 1);
  std::allocator_traits<A2>::construct(channel_allocator, channel, capacity,
                                       std::move(buffer_allocator));
  Sender<T, A1, A2> sender(channel, channel_allocator);
  Receiver<T, A1, A2> receiver(std::move(channel),
                               std::move(channel_allocator));
  return {std::move(sender), std::move(receiver)};
}
} // namespace chan::spmc::bounded

#endif
