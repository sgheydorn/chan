#include <chan/mpsc/unbounded/channel.hpp>
#include <test_util/leftover.hpp>

int main() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<std::unique_ptr<int>, 2>();
  return test_util::leftover(std::move(tx), std::move(rx));
}
