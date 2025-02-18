#include <chan/mpsc/unbounded/channel.hpp>
#include <test_util/many_to_one_disconnect_receiver.hpp>

int main() {
  auto [tx, rx] = chan::mpsc::unbounded::channel<int, 1>();
  return test_util::many_to_one_disconnect_receiver(std::move(tx),
                                                    std::move(rx));
}
