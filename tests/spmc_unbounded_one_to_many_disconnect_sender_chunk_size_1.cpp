#include <chan/spmc/unbounded/channel.hpp>
#include <test_util/one_to_many_disconnect_sender.hpp>

int main() {
  auto [tx, rx] = chan::spmc::unbounded::channel<int, 1>();
  return test_util::one_to_many_disconnect_sender(std::move(tx), std::move(rx));
}
