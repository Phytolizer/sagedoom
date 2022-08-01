#include "doom/error.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

noreturn void doom_error(const char* message, ...) {
  va_list args;
  va_start(args, message);
  vprintf(message, args);
  va_end(args);
  printf("\n");
  exit(1);
}
