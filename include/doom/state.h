#pragma once

#include <stdbool.h>

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

typedef struct {
  DoomKeys keys;
  DoomMouse mouse;
  DoomPlayer player;
  DoomFrameBuffer frame_buffer;
} DoomState;
