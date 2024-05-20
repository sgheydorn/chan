#ifndef _CHAN_TRY_SEND_ERROR_KIND_H
#define _CHAN_TRY_SEND_ERROR_KIND_H

namespace chan {
enum class TrySendErrorKind {
  Full,
  Disconnected,
};
}

#endif
