#include <stdio.h>
#include <stdlib.h>

#include "test-util.h"

void check_fail(const char* message) {
  puts(message);
  abort();
}
