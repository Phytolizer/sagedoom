#include "doom/wad.h"

#include "doom/error.h"
#include "doom/file_util.h"
#include "doom/misc.h"
#include "str_ext.h"

#include <ctype.h>
#include <stdlib.h>
#include <str.h>
#include <string.h>

static void l_add_file(DoomState* state, const char* filename);
static void l_extract_file_base(str filename, char* dest);

void wad_init_multiple_files(DoomState* state, const char* const* filenames) {
  state->wad.num_lumps = 0;
  state->wad.lump_info = NULL;

  for (size_t i = 0; filenames[i] != NULL; i++) {
    l_add_file(state, filenames[i]);
  }

  if (state->wad.num_lumps == 0) {
    doom_error("wad_init_multiple_files: No files found");
  }

  state->wad.lump_cache = calloc(state->wad.num_lumps, sizeof(void*));

  if (state->wad.lump_cache == NULL) {
    doom_error("wad_init_multiple_files: Failed to allocate lump cache");
  }
}

void* cache_lump_name(DoomState* state, const char* name, DoomPurgeTag tag) {
  TODO("Cache lump by name");
  return NULL;
}

static void l_add_file(DoomState* state, const char* filename) {
  FILE* handle = fopen(filename, "rb");
  if (handle == NULL) {
    printf(" Failed to open file %s\n", filename);
    return;
  }

  printf(" Adding file %s\n", filename);
  size_t start_lump = state->wad.num_lumps;

  str filename_str = str_ref(filename);
  str filename_ext = str_ref(str_end(filename_str) - 4);
  DoomFileLump* file_info;
  bool free_file_info = false;
  DoomFileLump single_info;
  if (!str_has_suffix_ci(filename_str, str_lit(".wad"))) {
    // single lump
    file_info = &single_info;
    single_info.file_pos = 0;
    single_info.size = (int32_t)file_length(handle);
    l_extract_file_base(filename_str, single_info.name);
    ++state->wad.num_lumps;
  } else {
    DoomWadInfo header;
    (void)fread(&header, sizeof(DoomWadInfo), 1, handle);
    if (!str_eq(str_ref_chars(header.identification, 4), str_lit("IWAD")) &&
        !str_eq(str_ref_chars(header.identification, 4), str_lit("PWAD"))) {
      doom_error("l_add_file: File is missing IWAD or PWAD id");
    }
    size_t length = header.num_lumps * sizeof(DoomFileLump);
    file_info = malloc(length);
    free_file_info = true;
    (void)fseek(handle, header.info_table_ofs, SEEK_SET);
    (void)fread(file_info, length, 1, handle);
    state->wad.num_lumps += header.num_lumps;
  }
  state->wad.lump_info = realloc(
      state->wad.lump_info, state->wad.num_lumps * sizeof(DoomLumpInfo));
  if (state->wad.lump_info == NULL) {
    doom_error("l_add_file: Failed to allocate lump info");
  }

  DoomLumpInfo* lump_p = &state->wad.lump_info[start_lump];

  for (size_t i = start_lump; i < state->wad.num_lumps; ++i) {
    lump_p->handle = handle;
    lump_p->position = file_info->file_pos;
    lump_p->size = file_info->size;
    strncpy(lump_p->name, file_info->name, 8);
    ++lump_p;
    ++file_info;
  }
}

static void l_extract_file_base(str filename, char* dest) {
  const char* src = str_end(filename) - 1;
  while (src > str_ptr(filename) && *src != '/' && *src != '\\') {
    --src;
  }
  memset(dest, 0, 8);
  size_t length = 0;
  while (src < str_end(filename) && *src != '.') {
    if (length == 8) {
      doom_error("l_extract_file_base: File name is too long");
    }
    dest[length] = (char)toupper(*src);
    ++length;
    ++src;
  }
}
