#include <chan/mpmc/unbounded/channel.hpp>
#include <test_util/many_to_one_disconnect_sender.hpp>

int main() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int, 16>();
  return test_util::many_to_one_disconnect_sender(std::move(tx), std::move(rx));
}
