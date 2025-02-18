#include <chan/spmc/unbuffered/channel.hpp>
#include <test_util/add_remove_receiver.hpp>

int main() {
  auto [tx, rx] = chan::spmc::unbuffered::channel<int>();
  return test_util::add_remove_receiver(std::move(tx), std::move(rx));
}
