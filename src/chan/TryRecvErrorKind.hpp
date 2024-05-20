#ifndef _CHAN_TRY_RECV_ERROR_KIND_H
#define _CHAN_TRY_RECV_ERROR_KIND_H

namespace chan {
enum class TryRecvErrorKind {
  Empty,
  Disconnected,
};
}

#endif
