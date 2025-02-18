#include <chan/spmc/bounded/channel.hpp>
#include <test_util/two_consecutive.hpp>

int main() {
  auto [tx, rx] = chan::spmc::bounded::channel<int>(16);
  return test_util::two_consecutive(std::move(tx), std::move(rx));
}
