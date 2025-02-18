#include <chan/mpsc/unbuffered/channel.hpp>
#include <test_util/unbuffered_disconnect_sender.hpp>

int main() {
  auto [tx, rx] = chan::mpsc::unbuffered::channel<int>();
  return test_util::unbuffered_disconnect_sender(std::move(tx), std::move(rx));
}
