#include <chan/mpmc/unbuffered/channel.hpp>
#include <test_util/add_remove_sender.hpp>

int main() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  return test_util::add_remove_sender(std::move(tx), std::move(rx));
}
