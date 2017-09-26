#include <stdint.h>
#include <stdlib.h>

#include "util.h"

void* util_safe_container_of_helper(void* ptr, size_t offset) {
  if (ptr == NULL)
    return NULL;
  else
    return (char*) ptr - offset;
}
