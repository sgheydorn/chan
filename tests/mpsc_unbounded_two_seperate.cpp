#include <chan/mpsc/unbounded/channel.hpp>
#include <test_util/two_seperate.hpp>

int main() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int, 16>();
  return test_util::two_seperate(std::move(tx), std::move(rx));
}
