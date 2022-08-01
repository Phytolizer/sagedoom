#include "doom/intermission.h"

#include "doom/misc.h"

static void l_draw_stats(DoomState* state);
static void l_draw_show_next_loc(DoomState* state);
static void l_draw_no_state(DoomState* state);

void render_intermission(DoomState* state) {
  switch (state->intermission_state) {
    case DOOM_INTERMISSION_STATE_STAT_COUNT:
      l_draw_stats(state);
      break;
    case DOOM_INTERMISSION_STATE_SHOW_NEXT_LOC:
      l_draw_show_next_loc(state);
      break;
    case DOOM_INTERMISSION_STATE_NO_STATE:
      l_draw_no_state(state);
      break;
  }
}

static void l_draw_stats(DoomState* state) {
  TODO("Draw stats");
}
static void l_draw_show_next_loc(DoomState* state) {
  TODO("Draw when showing next loc");
}
static void l_draw_no_state(DoomState* state) {
  TODO("Draw when no state");
}
