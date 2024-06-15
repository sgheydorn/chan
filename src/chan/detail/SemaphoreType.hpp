#ifndef _CHAN_DETAIL_SEMAPHORE_TYPE_H_
#define _CHAN_DETAIL_SEMAPHORE_TYPE_H_

#ifdef CHAN_REPLACE_SEMAPHORE_WITH_CVAR
#include "CvarSemaphore.hpp"
namespace chan::detail {
using SemaphoreType = CvarSemaphore;
}
#else
#include <semaphore>
namespace chan::detail {
using SemaphoreType = ::std::counting_semaphore<>;
}
#endif

#endif
