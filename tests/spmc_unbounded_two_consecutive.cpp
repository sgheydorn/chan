#include <chan/spmc/unbounded/channel.hpp>
#include <test_util/two_consecutive.hpp>

int main() {
  auto [tx, rx] = chan::spmc::unbounded::channel<int, 16>();
  return test_util::two_consecutive(std::move(tx), std::move(rx));
}
