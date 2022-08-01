#include "doom/render.h"

#include "doom/intermission.h"
#include "doom/misc.h"
#include "doom/video.h"
#include "doom/wad.h"

static void l_render_automap(DoomState* state);
static void l_render_stbar(DoomState* state);
static void l_render_finale(DoomState* state);
static void l_render_demoscreen(DoomState* state);
static void l_render_player_view(DoomState* state);
static void l_render_hud(DoomState* state);
static void l_render_menu(DoomState* state);

void render_frame(DoomState* state) {
  switch (state->game_state) {
    case DOOM_GAME_STATE_LEVEL:
      if (state->automap_active) {
        l_render_automap(state);
      }
      l_render_stbar(state);
      break;
    case DOOM_GAME_STATE_INTERMISSION:
      render_intermission(state);
      break;
    case DOOM_GAME_STATE_FINALE:
      l_render_finale(state);
      break;
    case DOOM_GAME_STATE_DEMOSCREEN:
      l_render_demoscreen(state);
      break;
    case DOOM_GAME_STATE_WIPE:
      // handled elsewhere
      break;
  }

  if (state->game_state == DOOM_GAME_STATE_LEVEL && !state->automap_active &&
      state->game_tic != 0) {
    l_render_player_view(state);
  }

  l_render_hud(state);

  set_palette(
      state, cache_lump_name(state, str_lit("PLAYPAL"), PURGE_TAG_CACHE));

  l_render_menu(state);
}

static void l_render_automap(DoomState* state) {
  TODO("Render automap");
}
static void l_render_stbar(DoomState* state) {
  TODO("Render status bar");
}
static void l_render_finale(DoomState* state) {
  TODO("Render finale");
}
static void l_render_demoscreen(DoomState* state) {
  TODO("Render demo screen");
}
static void l_render_player_view(DoomState* state) {
  TODO("Render player view");
}
static void l_render_hud(DoomState* state) {
  TODO("Render HUD");
}
static void l_render_menu(DoomState* state) {
  TODO("Render menu");
}
