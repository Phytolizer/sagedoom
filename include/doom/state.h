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

typedef struct {
  DoomKeys keys;
  DoomMouse mouse;
  DoomPlayer player;
} DoomState;
