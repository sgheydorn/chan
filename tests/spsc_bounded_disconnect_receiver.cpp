#include <chan/spsc/bounded/channel.hpp>
#include <test_util/disconnect_receiver.hpp>

int main() {
  auto [tx, rx] = chan::spsc::bounded::channel<int>(16);
  return test_util::disconnect_receiver(std::move(tx), std::move(rx));
}
