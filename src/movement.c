#include "doom/movement.h"

#include "doom/framerate.h"
#include "doom/math.h"

#include <math.h>

void handle_movement(DoomState* state) {
  state->player.ang += state->mouse.dx;
  while (state->player.ang > 360) {
    state->player.ang -= 360;
  }
  while (state->player.ang < 0) {
    state->player.ang += 360;
  }

  double dx = cos(deg2rad(state->player.ang));
  double dy = sin(deg2rad(state->player.ang));
  if (state->keys.w) {
    state->player.x += dx * DOOM_MOVEMENT_PER_FRAME;
    state->player.y += dy * DOOM_MOVEMENT_PER_FRAME;
  }
  if (state->keys.s) {
    state->player.x -= dx * DOOM_MOVEMENT_PER_FRAME;
    state->player.y -= dy * DOOM_MOVEMENT_PER_FRAME;
  }
  if (state->keys.a) {
    state->player.x += dy * DOOM_MOVEMENT_PER_FRAME;
    state->player.y -= dx * DOOM_MOVEMENT_PER_FRAME;
  }
  if (state->keys.d) {
    state->player.x -= dy * DOOM_MOVEMENT_PER_FRAME;
    state->player.y += dx * DOOM_MOVEMENT_PER_FRAME;
  }
}
