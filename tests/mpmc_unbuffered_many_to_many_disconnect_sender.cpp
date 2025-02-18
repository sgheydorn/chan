#include <chan/mpmc/unbuffered/channel.hpp>
#include <test_util/many_to_many_disconnect_sender.hpp>

int main() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  return test_util::many_to_many_disconnect_sender(std::move(tx),
                                                   std::move(rx));
}
