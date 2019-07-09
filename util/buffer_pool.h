#ifndef UTIL_BUFFER_POOL_H
#define UTIL_BUFFER_POOL_H

#include <pthread.h>
#include "util/buffer.h"

namespace util {
  class buffer_pool {
    public:
      // Constructor.
      buffer_pool() = default;

      // Destructor.
      ~buffer_pool();

      // Create.
      bool create(size_t size, size_t buffer_size);

      // Push buffer.
      bool push(buffer* b);

      // Pop buffer.
      buffer* pop();

      // Pop buffer (timeout in milliseconds).
      buffer* pop(unsigned timeout);

    private:
      buffer** _M_buffers = nullptr;
      size_t _M_size = 0;
      size_t _M_used = 0;

      // Mutex.
      pthread_mutex_t _M_mutex;
      bool _M_mutex_initialized = false;

      // Condition variable.
      pthread_cond_t _M_cond;
      bool _M_cond_initialized = false;
  };
}

#endif // UTIL_BUFFER_POOL_H
