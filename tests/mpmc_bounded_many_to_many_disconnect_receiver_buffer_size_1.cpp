#include <chan/mpmc/bounded/channel.hpp>
#include <test_util/many_to_many_disconnect_receiver.hpp>

int main() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(1);
  return test_util::many_to_many_disconnect_receiver(std::move(tx),
                                                     std::move(rx));
}
