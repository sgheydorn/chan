#ifndef _CHAN_DETAIL_CVAR_SEMAPHORE_H
#define _CHAN_DETAIL_CVAR_SEMAPHORE_H

#include <condition_variable>

class CvarSemaphore {
  std::condition_variable cvar;
  std::mutex mutex;
  std::ptrdiff_t count;

public:
  CvarSemaphore(std::ptrdiff_t desired) : count(desired) {}

  void acquire() {
    std::unique_lock lock(this->mutex);
    this->cvar.wait(lock, [this] { return this->count > 0; });
    --this->count;
  }

  bool try_acquire() {
    std::lock_guard _lock(this->mutex);
    if (this->count == 0) {
      return false;
    }
    --this->count;
    return true;
  }

  template <typename Rep, typename Period>
  bool try_acquire_for(const std::chrono::duration<Rep, Period> &rel_time) {
    std::unique_lock lock(this->mutex);
    if (!this->cvar.wait_for(lock, rel_time,
                             [this] { return this->count > 0; })) {
      return false;
    }
    --this->count;
    return true;
  }

  template <typename Clock, typename Duration>
  bool
  try_acquire_until(const std::chrono::time_point<Clock, Duration> &abs_time) {
    std::unique_lock lock(this->mutex);
    if (!this->cvar.wait_until(lock, abs_time,
                               [this] { return this->count > 0; })) {
      return false;
    }
    --this->count;
    return true;
  }

  void release(std::ptrdiff_t update = 1) {
    {
      std::lock_guard _lock(this->mutex);
      this->count += update;
    }
    if (update > 1) {
      this->cvar.notify_all();
    } else if (update == 1) {
      this->cvar.notify_one();
    }
  }
};

#endif
