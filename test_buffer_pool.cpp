#include <stdlib.h>
#include <stdio.h>
#include "util/buffer_pool.h"

int main()
{
  util::buffer_pool pool;
  if (pool.create(100, 1024)) {
    return 0;
  } else {
    fprintf(stderr, "Error creating buffer pool.\n");

    return -1;
  }
}
