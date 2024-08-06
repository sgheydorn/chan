#ifndef _CHAN_DETAIL_SEMAPHORE_TYPE_H
#define _CHAN_DETAIL_SEMAPHORE_TYPE_H

#ifdef CHAN_REPLACE_SEMAPHORE_WITH_CONDITION_VARIABLE
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
