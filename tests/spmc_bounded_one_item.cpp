#include <chan/spmc/bounded/channel.hpp>
#include <test_util/one_item.hpp>

int main() {
  auto [tx, rx] = chan::spmc::bounded::channel<int>(16);
  return test_util::one_item(std::move(tx), std::move(rx));
}
