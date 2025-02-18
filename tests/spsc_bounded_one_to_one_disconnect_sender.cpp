#include <chan/spsc/bounded/channel.hpp>
#include <test_util/one_to_one_disconnect_sender.hpp>

int main() {
  auto [tx, rx] = chan::spsc::bounded::channel<int>(16);
  return test_util::one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}
