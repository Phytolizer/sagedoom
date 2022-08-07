#pragma once

#include "doom/renderer/types.h"

#include <stdbool.h>

typedef struct {
  // upper-right-hand corner of the number
  // (right-justified)
  int x;
  int y;
  // maximum number of digits
  int width;
  // last number value
  int old_value;
  // pointer to current value
  int* value;
  // whether to update number
  bool* should_update;
  // list of patches for numbers 0-9
  Patch** patches;
  // user data
  int user_data;
} StatusNumberWidget;

typedef enum { ST_NO_REFRESH, ST_REFRESH } StatusBarRefresh;
