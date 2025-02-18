#include <chan/mpmc/unbuffered/channel.hpp>
#include <test_util/unbuffered_try.hpp>

int main() {
  auto [tx, rx] = chan::mpmc::unbuffered::channel<int>();
  return test_util::unbuffered_try(std::move(tx), std::move(rx));
}
