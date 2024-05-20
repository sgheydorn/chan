#include "Channel.hpp"
#include "Receiver.hpp"
#include "Sender.hpp"

namespace chan::mpmc::unbuffered {
template <typename T, typename A = std::allocator<Channel<T>>>
std::pair<Sender<T, A>, Receiver<T, A>> create(A allocator = A()) {
  auto channel = std::allocator_traits<A>::allocate(allocator, 1);
  std::allocator_traits<A>::construct(allocator, channel);
  Sender<T, A> sender(channel, allocator);
  Receiver<T, A> receiver(std::move(channel), std::move(allocator));
  return {std::move(sender), std::move(receiver)};
}
} // namespace chan::mpmc::unbuffered
