#include <chan/mpmc/bounded/channel.hpp>
#include <test_util/two_seperate.hpp>

int main() {
  auto [tx, rx] = chan::mpmc::bounded::channel<int>(16);
  return test_util::two_seperate(std::move(tx), std::move(rx));
}
