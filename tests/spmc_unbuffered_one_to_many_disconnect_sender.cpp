#include <chan/spmc/unbuffered/channel.hpp>
#include <test_util/one_to_many_disconnect_sender.hpp>

int main() {
  auto [tx, rx] = chan::spmc::unbuffered::channel<int>();
  return test_util::one_to_many_disconnect_sender(std::move(tx), std::move(rx));
}
