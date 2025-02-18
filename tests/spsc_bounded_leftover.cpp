#include <chan/spsc/bounded/channel.hpp>
#include <test_util/leftover.hpp>

int main() {
  auto [tx, rx] = chan::spsc::bounded::channel<std::unique_ptr<int>>(16);
  return test_util::leftover(std::move(tx), std::move(rx));
}
