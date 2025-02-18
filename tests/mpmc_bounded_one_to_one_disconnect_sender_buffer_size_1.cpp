#include <chan/mpmc/bounded/channel.hpp>
#include <test_util/one_to_one_disconnect_sender.hpp>

int main() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(1);
  return test_util::one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}
