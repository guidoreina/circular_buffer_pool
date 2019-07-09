#include <stdlib.h>
#include <stdio.h>
#include "util/circular_buffer_pool.h"

int main()
{
  util::circular_buffer_pool pool;
  if (pool.create(100, 1024)) {
    return 0;
  } else {
    fprintf(stderr, "Error creating buffer pool.\n");

    return -1;
  }
}
