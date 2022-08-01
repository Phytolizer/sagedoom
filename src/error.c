#include "doom/error.h"

#include <stdio.h>
#include <stdlib.h>

void doom_error(const char* message) {
  printf("%s\n", message);
  exit(1);
}
