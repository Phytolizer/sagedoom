#pragma once

#include "doom/resolution.h"
#include "doom/state.h"
#include "doom/statusbar/types.h"

#include <stdbool.h>

enum {
  ST_HEIGHT = 32 * DOOM_RES_SCALE,
  ST_WIDTH = DOOM_WIDTH,
  ST_Y = DOOM_HEIGHT - ST_HEIGHT,
};

void stlib_update_num(
    DoomState* state, StatusNumberWidget* widget, StatusBarRefresh refresh);
