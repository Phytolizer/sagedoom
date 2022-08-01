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
  DoomKeys keys;
  DoomMouse mouse;
} DoomState;
