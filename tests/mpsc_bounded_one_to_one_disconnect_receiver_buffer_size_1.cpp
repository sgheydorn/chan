#include <chan/mpsc/bounded/channel.hpp>
#include <test_util/one_to_one_disconnect_receiver.hpp>

int main() {
  auto [tx, rx] = chan::mpsc::bounded::channel<int>(1);
  return test_util::one_to_one_disconnect_receiver(std::move(tx),
                                                   std::move(rx));
}
