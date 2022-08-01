#pragma once

#include <stdio.h>
#include <stdlib.h>

#define TODO(Msg) \
  do { \
    printf("TODO: %s\n", Msg); \
    exit(1); \
  } while (0)
