#include <chan/mpmc/unbuffered/channel.hpp>
#include <test_util/one_to_one_disconnect_sender.hpp>

int main() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  return test_util::one_to_one_disconnect_sender(std::move(tx), std::move(rx));
}
