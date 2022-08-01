#pragma once

#define GAME_MODES_X \
  X(SHAREWARE) \
  X(REGISTERED) \
  X(COMMERCIAL) \
  X(RETAIL) \
  X(UNDETERMINED)

typedef enum {
#define X(x) GAME_MODE_##x,
  GAME_MODES_X
#undef X
} GameMode;
