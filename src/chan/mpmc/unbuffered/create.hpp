#ifndef _CHAN_MPMC_UNBUFFERED_CREATE_H
#define _CHAN_MPMC_UNBUFFERED_CREATE_H

#include "Channel.hpp"
#include "Receiver.hpp"
#include "Sender.hpp"

namespace chan::mpmc::unbuffered {
template <typename T, typename A1 = std::allocator<std::optional<T> *>,
          typename A2 = std::allocator<Channel<T, A1>>>
std::pair<Sender<T, A1, A2>, Receiver<T, A1, A2>>
create(A1 packet_allocator = A1(), A2 channel_allocator = A2()) {
  auto channel = std::allocator_traits<A2>::allocate(channel_allocator, 1);
  std::allocator_traits<A2>::construct(channel_allocator, channel,
                                       std::move(packet_allocator));
  Sender<T, A1, A2> sender(channel, channel_allocator);
  Receiver<T, A1, A2> receiver(std::move(channel),
                               std::move(channel_allocator));
  return {std::move(sender), std::move(receiver)};
}
} // namespace chan::mpmc::unbuffered

#endif
