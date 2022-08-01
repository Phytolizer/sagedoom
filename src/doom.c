#include "doom/doom.h"

void doom_add_file(DoomState* state, str file) {
  size_t num_wad_files = 0;
  while (num_wad_files < MAX_WAD_FILES &&
      !str_is_empty(state->wad_files[num_wad_files])) {
    ++num_wad_files;
  }

  str_cat(&state->wad_files[num_wad_files], file);
}
