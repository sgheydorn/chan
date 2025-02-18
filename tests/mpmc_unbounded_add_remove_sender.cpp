#include <chan/mpmc/unbounded/channel.hpp>
#include <test_util/add_remove_sender.hpp>

int main() {
  auto [tx, rx] = chan::mpmc::unbounded::channel<int, 1>();
  return test_util::add_remove_sender(std::move(tx), std::move(rx));
}
