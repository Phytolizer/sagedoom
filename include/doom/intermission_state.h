#pragma once

#define DOOM_INTERMISSION_STATES_X \
  X(STAT_COUNT) \
  X(SHOW_NEXT_LOC) \
  X(NO_STATE)

typedef enum {
#define X(x) DOOM_INTERMISSION_STATE_##x,
  DOOM_INTERMISSION_STATES_X
#undef X
} DoomIntermissionState;
