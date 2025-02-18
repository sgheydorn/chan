#include <chan/spsc/unbounded/channel.hpp>
#include <test_util/one_to_one_disconnect_receiver.hpp>

int main() {
  auto [tx, rx] = chan::spsc::unbounded::channel<int, 1>();
  return test_util::one_to_one_disconnect_receiver(std::move(tx),
                                                   std::move(rx));
}
