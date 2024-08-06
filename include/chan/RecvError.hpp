#ifndef _CHAN_RECV_ERROR_H
#define _CHAN_RECV_ERROR_H

namespace chan {
/// Error for the `recv` operation. Occurs when there are no remaining items or
/// senders.
struct RecvError {};
} // namespace chan

#endif
