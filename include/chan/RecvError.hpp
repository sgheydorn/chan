#ifndef _CHAN_RECV_ERROR_HPP
#define _CHAN_RECV_ERROR_HPP

namespace chan {
/// Error for the `recv` operation. Occurs when there are no remaining items or
/// senders.
struct RecvError {};
} // namespace chan

#endif
