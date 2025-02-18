#include <chan/spsc/unbounded/channel.hpp>
#include <test_util/unbounded_disconnect_receiver.hpp>

int main() {
  auto [tx, rx] = chan::spsc::unbounded::channel<int, 16>();
  return test_util::unbounded_disconnect_receiver(std::move(tx), std::move(rx));
}
