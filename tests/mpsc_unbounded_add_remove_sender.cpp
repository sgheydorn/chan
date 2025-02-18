#include <chan/mpsc/unbounded/channel.hpp>
#include <test_util/add_remove_sender.hpp>

int main() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int, 1>();
  return test_util::add_remove_sender(std::move(tx), std::move(rx));
}
