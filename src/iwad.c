#include "doom/iwad.h"

#include "doom/doom.h"
#include "doom/error.h"
#include "doom/parm.h"
#include "str_ext.h"

#include <stdlib.h>
#include <str.h>
#include <unistd.h>

void identify_version(DoomState* state) {
  const char* doom_wad_dir_c = getenv("DOOMWADDIR");
  if (doom_wad_dir_c == NULL) {
    doom_wad_dir_c = ".";
  }
  str doom_wad_dir = str_ref(doom_wad_dir_c);

  str doom2_wad = str_null;
  str_cat(&doom2_wad, doom_wad_dir, str_lit("/doom2.wad"));

  str doomu_wad = str_null;
  str_cat(&doomu_wad, doom_wad_dir, str_lit("/doomu.wad"));

  str doom_wad = str_null;
  str_cat(&doom_wad, doom_wad_dir, str_lit("/doom.wad"));

  str doom1_wad = str_null;
  str_cat(&doom1_wad, doom_wad_dir, str_lit("/doom1.wad"));

  str plutonia_wad = str_null;
  str_cat(&plutonia_wad, doom_wad_dir, str_lit("/plutonia.wad"));

  str tnt_wad = str_null;
  str_cat(&tnt_wad, doom_wad_dir, str_lit("/tnt.wad"));

#define CLEANUP() \
  do { \
    str_free(tnt_wad); \
    str_free(plutonia_wad); \
    str_free(doom1_wad); \
    str_free(doom_wad); \
    str_free(doomu_wad); \
    str_free(doom2_wad); \
  } while (false)

  const char* home_c = getenv("HOME");
  if (home_c == NULL) {
    doom_error("Please set $HOME to your home directory");
  }
  str home = str_ref(home_c);

  if (!access(str_ptr(doom2_wad), R_OK)) {
    state->game_mode = GAME_MODE_COMMERCIAL;
    doom_add_file(state, doom2_wad);
    CLEANUP();
    return;
  }

  if (!access(str_ptr(plutonia_wad), R_OK)) {
    state->game_mode = GAME_MODE_COMMERCIAL;
    doom_add_file(state, plutonia_wad);
    CLEANUP();
    return;
  }

  if (!access(str_ptr(tnt_wad), R_OK)) {
    state->game_mode = GAME_MODE_COMMERCIAL;
    doom_add_file(state, tnt_wad);
    CLEANUP();
    return;
  }

  if (!access(str_ptr(doomu_wad), R_OK)) {
    state->game_mode = GAME_MODE_RETAIL;
    doom_add_file(state, doomu_wad);
    CLEANUP();
    return;
  }

  if (!access(str_ptr(doom_wad), R_OK)) {
    state->game_mode = GAME_MODE_REGISTERED;
    doom_add_file(state, doom_wad);
    CLEANUP();
    return;
  }

  if (!access(str_ptr(doom1_wad), R_OK)) {
    state->game_mode = GAME_MODE_SHAREWARE;
    doom_add_file(state, doom1_wad);
    CLEANUP();
    return;
  }

  MaybeSize maybe_iwad_parm_idx = parm_index(state, str_lit("-iwad"));
  if (maybe_iwad_parm_idx.is_present &&
      maybe_iwad_parm_idx.value + 1 < state->argc) {
    str iwad_parm = str_ref(state->argv[maybe_iwad_parm_idx.value + 1]);
    if (!access(str_ptr(iwad_parm), R_OK)) {
      if (str_has_suffix_ci(iwad_parm, str_lit("doom2.wad")) ||
          str_has_suffix_ci(iwad_parm, str_lit("plutonia.wad")) ||
          str_has_suffix_ci(iwad_parm, str_lit("tnt.wad"))) {
        state->game_mode = GAME_MODE_COMMERCIAL;
      } else if (str_has_suffix_ci(iwad_parm, str_lit("doomu.wad"))) {
        state->game_mode = GAME_MODE_RETAIL;
      } else if (str_has_suffix_ci(iwad_parm, str_lit("doom.wad"))) {
        state->game_mode = GAME_MODE_REGISTERED;
      } else if (str_has_suffix_ci(iwad_parm, str_lit("doom1.wad"))) {
        state->game_mode = GAME_MODE_SHAREWARE;
      } else {
        state->game_mode = GAME_MODE_UNDETERMINED;
      }
      doom_add_file(state, iwad_parm);
      CLEANUP();
      return;
    }
  }

  // don't error just yet
  state->game_mode = GAME_MODE_UNDETERMINED;
  CLEANUP();

#undef CLEANUP
}
