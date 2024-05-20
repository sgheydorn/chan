#ifndef _CHAN_SEND_ERROR_H
#define _CHAN_SEND_ERROR_H

namespace chan {
template <typename T> struct SendError {
  T item;
};
} // namespace chan

#endif
