#include <chan/spmc/bounded/channel.hpp>
#include <test_util/add_remove_receiver.hpp>

int main() {
  auto [tx, rx] = chan::spmc::bounded::channel<int>(1);
  return test_util::add_remove_receiver(std::move(tx), std::move(rx));
}
