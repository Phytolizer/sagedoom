#pragma once

#include <stdbool.h>

#define MAYBE_T(T) \
  struct { \
    bool is_present; \
    T value; \
  }

#define MAYBE_NONE(M) \
  (M) { \
    .is_present = false \
  }

#define MAYBE_SOME(M, V) \
  (M) { \
    .is_present = true, .value = (V) \
  }
