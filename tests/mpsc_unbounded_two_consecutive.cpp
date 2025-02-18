#include <chan/mpsc/unbounded/channel.hpp>
#include <test_util/two_consecutive.hpp>

int main() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int, 16>();
  return test_util::two_consecutive(std::move(tx), std::move(rx));
}
