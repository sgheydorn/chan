#ifndef _CHAN_DEFAULT_CHUNK_SIZE_H
#define _CHAN_DEFAULT_CHUNK_SIZE_H

#include <cstddef>

namespace chan {
/// Default CHUNK_SIZE for unbounded channels.
inline constexpr std::size_t DEFAULT_CHUNK_SIZE = 32;
} // namespace chan

#endif
