#include <time.h>
#include "util/circular_buffer_pool.h"

util::circular_buffer_pool::~circular_buffer_pool()
{
  if (_M_buffers) {
    for (size_t i = 0; i < _M_size; i++) {
      free(_M_buffers[i]);
    }

    free(_M_buffers);
  }

  if (_M_cond_initialized) {
    pthread_cond_destroy(&_M_cond);
  }

  if (_M_mutex_initialized) {
    pthread_mutex_destroy(&_M_mutex);
  }
}

bool util::circular_buffer_pool::create(size_t size, size_t buffer_size)
{
  // Initialize mutex.
  if (pthread_mutex_init(&_M_mutex, nullptr) == 0) {
    _M_mutex_initialized = true;

    // Initialize condition variable.
    if (pthread_cond_init(&_M_cond, nullptr) == 0) {
      _M_cond_initialized = true;

      // Create buffers.
      if ((_M_buffers = static_cast<buffer**>(
                          malloc(size * sizeof(buffer*))
                        )) != nullptr) {
        for (_M_size = 0; _M_size < size; _M_size++) {
          if ((_M_buffers[_M_size] = buffer::create(buffer_size)) == nullptr) {
            return false;
          }
        }

        return true;
      }
    }
  }

  return false;
}

bool util::circular_buffer_pool::push(buffer* b)
{
  pthread_mutex_lock(&_M_mutex);

  // If the buffer pool is not full...
  if (_M_used < _M_size) {
    _M_buffers[_M_next] = b;

    _M_next = (_M_next + 1) % _M_size;

    _M_used++;

    pthread_cond_signal(&_M_cond);

    pthread_mutex_unlock(&_M_mutex);

    return true;
  } else {
    pthread_mutex_unlock(&_M_mutex);

    return false;
  }
}

util::buffer* util::circular_buffer_pool::pop()
{
  pthread_mutex_lock(&_M_mutex);

  // If the buffer pool is not empty...
  if (_M_used > 0) {
    buffer* b = _M_buffers[_M_first];

    _M_first = (_M_first + 1) % _M_size;

    _M_used--;

    pthread_mutex_unlock(&_M_mutex);

    return b;
  }

  pthread_mutex_unlock(&_M_mutex);

  return nullptr;
}

util::buffer* util::circular_buffer_pool::pop(unsigned timeout)
{
  pthread_mutex_lock(&_M_mutex);

  // If the buffer pool is not empty...
  if (_M_used > 0) {
    buffer* b = _M_buffers[_M_first];

    _M_first = (_M_first + 1) % _M_size;

    _M_used--;

    pthread_mutex_unlock(&_M_mutex);

    return b;
  }

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);

  ts.tv_sec += (timeout / 1000);

  long ns = ts.tv_nsec + ((timeout % 1000) * 1000000);

  if (ns >= 1000000000l) {
    ts.tv_sec++;
  }

  ts.tv_nsec = ns % 1000000000l;

  do {
    if (pthread_cond_timedwait(&_M_cond, &_M_mutex, &ts) != 0) {
      pthread_mutex_unlock(&_M_mutex);

      return nullptr;
    }
  } while (_M_used == 0);

  buffer* b = _M_buffers[_M_first];

  _M_first = (_M_first + 1) % _M_size;

  _M_used--;

  pthread_mutex_unlock(&_M_mutex);

  return b;
}
