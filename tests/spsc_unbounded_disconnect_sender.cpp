#include <chan/spsc/unbounded/channel.hpp>
#include <test_util/disconnect_sender.hpp>

int main() {
  auto [tx, rx] = chan::spsc::unbounded::channel<int, 16>();
  return test_util::disconnect_sender(std::move(tx), std::move(rx));
}
