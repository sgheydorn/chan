#ifndef _CHAN_SPSC_UNBUFFERED_CREATE_H
#define _CHAN_SPSC_UNBUFFERED_CREATE_H

#include "Chan.hpp"
#include "Receiver.hpp"
#include "Sender.hpp"

namespace chan::spsc::unbuffered {
template <typename T, typename A = std::allocator<Chan<T>>>
std::pair<Sender<T, A>, Receiver<T, A>> channel(A allocator = A()) {
  auto channel = std::allocator_traits<A>::allocate(allocator, 1);
  std::allocator_traits<A>::construct(allocator, channel);
  Sender<T, A> sender(channel, allocator);
  Receiver<T, A> receiver(std::move(channel), std::move(allocator));
  return {std::move(sender), std::move(receiver)};
}
} // namespace chan::spsc::unbuffered

#endif
