#pragma once

#include "doom/intermission_state.h"
#include "doom/wad_types.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  bool w;
  bool s;
  bool a;
  bool d;
} DoomKeys;

typedef struct {
  double px;
  double py;
  double dx;
  double dy;
} DoomMouse;

typedef struct {
  double x;
  double y;
  double z;
  double ang;
} DoomPlayer;

#define DOOM_FRAME_BUFFER_IDS_X \
  X(PRIMARY) \
  X(SCREENSHOT_AND_BG) \
  X(WIPE1) \
  X(WIPE2) \
  X(CLEAN_SBAR)

typedef enum {
#define X(x) DOOM_FRAME_BUFFER_ID_##x,
  DOOM_FRAME_BUFFER_IDS_X
#undef X
      DOOM_FRAME_BUFFER_IDS_COUNT
} DoomFrameBufferId;

typedef struct {
  float* screens[DOOM_FRAME_BUFFER_IDS_COUNT];
} DoomFrameBuffer;

#define DOOM_GAME_STATES_X \
  X(LEVEL) \
  X(INTERMISSION) \
  X(FINALE) \
  X(DEMOSCREEN) \
  X(WIPE)

typedef enum {
#define X(x) DOOM_GAME_STATE_##x,
  DOOM_GAME_STATES_X
#undef X
} DoomGameState;

typedef struct {
  void** lump_cache;
  DoomLumpInfo* lump_info;
  size_t num_lumps;
} DoomWadState;

typedef struct {
  DoomKeys keys;
  DoomMouse mouse;
  DoomPlayer player;
  DoomFrameBuffer frame_buffer;
  DoomGameState game_state;
  bool automap_active;
  size_t game_tic;
  DoomIntermissionState intermission_state;
  DoomWadState wad;
} DoomState;
