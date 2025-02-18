#include <chan/mpmc/bounded/channel.hpp>
#include <test_util/one_to_one_disconnect_receiver.hpp>

int main() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(16);
  return test_util::one_to_one_disconnect_receiver(std::move(tx),
                                                   std::move(rx));
}
