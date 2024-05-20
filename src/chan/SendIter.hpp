#ifndef _CHAN_SEND_ITER_H
#define _CHAN_SEND_ITER_H

#include <iterator>

namespace chan {
template <typename S> class SendIter {
public:
  using iterator_category = std::output_iterator_tag;
  using difference_type = std::ptrdiff_t;

private:
  S *sender;
  bool disconnected;

public:
  SendIter(S &sender) : sender(&sender) {}

  SendIter &operator=(S::Item item) {
    if (!this->sender->send(std::move(item))) {
      this->disconnected = true;
    }
    return *this;
  }

  SendIter &operator*() { return *this; }

  SendIter &operator++() { return *this; }

  SendIter &operator++(int) { return *this; }

  friend bool operator==(const SendIter &self, std::default_sentinel_t) {
    return self.disconnected;
  }
};
} // namespace chan

#endif
