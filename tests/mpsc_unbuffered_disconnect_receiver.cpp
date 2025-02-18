#include <chan/mpsc/unbuffered/channel.hpp>
#include <test_util/disconnect_receiver.hpp>

int main() {
  auto [tx, rx] = chan::mpsc::unbuffered::channel<int>();
  return test_util::disconnect_receiver(std::move(tx), std::move(rx));
}
