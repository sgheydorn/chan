#include <chan/mpmc/unbuffered/channel.hpp>
#include <test_util/one_to_many_disconnect_receiver.hpp>

int main() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  return test_util::one_to_many_disconnect_receiver(std::move(tx),
                                                    std::move(rx));
}
