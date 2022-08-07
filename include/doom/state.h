#pragma once

#include "doom/game_mode.h"
#include "doom/intermission_state.h"
#include "doom/statusbar/types.h"
#include "doom/wad_types.h"
#include "doom/zone_types.h"

#include <stdbool.h>
#include <stddef.h>
#include <str.h>

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

#define GAME_STATES_X \
  X(LEVEL) \
  X(INTERMISSION) \
  X(FINALE) \
  X(DEMOSCREEN) \
  X(WIPE)

typedef enum {
#define X(x) GAME_STATE_##x,
  GAME_STATES_X
#undef X
} GameState;

typedef struct {
  void** lump_cache;
  DoomLumpInfo* lump_info;
  size_t num_lumps;
} DoomWadState;

typedef struct {
  StatusNumberWidget ready_weapon;
} StatusWidgets;

typedef struct {
  bool first_time;
  bool status_bar_on;
  bool arms_on;
  StatusWidgets widgets;
} StatusBarState;

enum { MAX_WAD_FILES = 20 };

typedef struct {
  DoomKeys keys;
  DoomMouse mouse;
  DoomPlayer player;
  DoomFrameBuffer frame_buffer;
  GameState game_state;
  bool automap_active;
  size_t game_tic;
  DoomIntermissionState intermission_state;
  DoomWadState wad;
  MemoryZone* main_zone;
  GameMode game_mode;
  size_t argc;
  str* argv;
  bool dev_parm;
  str wad_files[MAX_WAD_FILES];
  bool modified_game;
  StatusBarState stbar;
} DoomState;
