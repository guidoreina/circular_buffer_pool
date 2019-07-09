#ifndef UTIL_BUFFER_H
#define UTIL_BUFFER_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace util {
  struct buffer {
    // Length of the data stored in 'data'.
    size_t len;

    // Data.
    uint8_t data[1];

    // Create.
    static buffer* create(size_t size);

    // Fill.
    void fill(const void* b, size_t l);
  };

  inline buffer* buffer::create(size_t size)
  {
    return static_cast<buffer*>(malloc(offsetof(struct buffer, data) + size));
  }

  inline void buffer::fill(const void* b, size_t l)
  {
    // It has been already checked that the data fits.
    memcpy(data, b, l);
    len = l;
  }
}

#endif // UTIL_BUFFER_H
