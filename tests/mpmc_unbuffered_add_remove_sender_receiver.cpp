#include <chan/mpmc/unbuffered/channel.hpp>
#include <test_util/add_remove_sender_receiver.hpp>

int main() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  return test_util::add_remove_sender_receiver(std::move(tx), std::move(rx));
}
