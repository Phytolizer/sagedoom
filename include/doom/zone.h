#pragma once

#define DOOM_PURGE_TAGS_X \
  /* not overwritten until freed */ \
  X(STATIC = 1) \
  X(SOUND = 2) \
  X(MUSIC = 3) \
  X(DAVE = 4) \
  X(LEVEL = 50) \
  X(LEVEL_SPECIAL = 51) \
  /* purgable when needed */ \
  X(PURGE_LEVEL = 100) \
  X(CACHE = 101)

typedef enum {
#define X(x) DOOM_PURGE_TAG_##x,
  DOOM_PURGE_TAGS_X
#undef X
} DoomPurgeTag;
