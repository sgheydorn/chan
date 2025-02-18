#include <chan/mpmc/bounded/channel.hpp>
#include <test_util/one_to_many_disconnect_sender.hpp>

int main() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(16);
  return test_util::one_to_many_disconnect_sender(std::move(tx), std::move(rx));
}
