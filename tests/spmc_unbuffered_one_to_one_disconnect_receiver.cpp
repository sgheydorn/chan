#include <chan/spmc/unbuffered/channel.hpp>
#include <test_util/one_to_one_disconnect_receiver.hpp>

int main() {
  auto [tx, rx] = chan::spmc::unbuffered::channel<int>();
  return test_util::one_to_one_disconnect_receiver(std::move(tx),
                                                   std::move(rx));
}
