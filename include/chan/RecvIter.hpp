#ifndef _CHAN_RECV_ITER_H
#define _CHAN_RECV_ITER_H

#include <iterator>
#include <optional>

#include "detail/expected_to_optional.hpp"

namespace chan {
/// Input iterator that receives items from a channel.
template <typename R> class RecvIter {
public:
  using iterator_category = std::input_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = R::Item;

private:
  const R *receiver;
  mutable std::optional<value_type> item;

  RecvIter(const R *receiver, std::optional<value_type> item)
      : receiver(receiver), item(std::move(item)) {}

public:
  RecvIter(const R &receiver)
      : receiver(&receiver),
        item(detail::expected_to_optional(receiver.recv())) {}

  value_type &operator*() const { return *this->item; }

  value_type *operator->() const { return &*this->item; }

  RecvIter &operator++() {
    this->item = detail::expected_to_optional(this->receiver->recv());
    return *this;
  }

  RecvIter operator++(int) {
    auto old_item = std::move(this->item);
    this->item = detail::expected_to_optional(this->receiver->recv());
    return RecvIter(this->channel, std::move(old_item));
  }

  friend bool operator==(const RecvIter &self, std::default_sentinel_t) {
    return !self.item;
  }
};
} // namespace chan

#endif
