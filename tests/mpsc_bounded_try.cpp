#include <chan/mpsc/bounded/channel.hpp>
#include <test_util/bounded_try.hpp>

int main() {
  auto [tx, rx] = chan::mpsc::bounded::channel<int>(16);
  return test_util::bounded_try(std::move(tx), std::move(rx), 16);
}
