#include "doom/wad.h"

#include "doom/error.h"
#include "doom/file_util.h"
#include "doom/misc.h"
#include "doom/types.h"
#include "str_ext.h"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <str.h>
#include <string.h>

static void l_add_file(DoomState* state, str filename);
static void l_extract_file_base(str filename, char* dest);

void wad_init_multiple_files(DoomState* state) {
  state->wad.num_lumps = 0;
  state->wad.lump_info = NULL;

  for (size_t i = 0; i < MAX_WAD_FILES && !str_is_empty(state->wad_files[i]);
       i++) {
    l_add_file(state, state->wad_files[i]);
  }

  if (state->wad.num_lumps == 0) {
    doom_error("wad_init_multiple_files: No files found");
  }

  state->wad.lump_cache = calloc(state->wad.num_lumps, sizeof(void*));

  if (state->wad.lump_cache == NULL) {
    doom_error("wad_init_multiple_files: Failed to allocate lump cache");
  }
}

void* cache_lump_num(DoomState* state, size_t lump, DoomPurgeTag tag) {
  if (lump >= state->wad.num_lumps) {
    doom_error("cache_lump_num: Lump %zu out of range", lump);
  }

  if (state->wad.lump_cache[lump] != NULL) {
    zone_change_tag(state->wad.lump_cache[lump], tag);
    return state->wad.lump_cache[lump];
  }

  Byte* ptr = zone_malloc(
      state, lump_length(state, lump), tag, &state->wad.lump_cache[lump]);
  read_lump(state, lump, ptr);
  state->wad.lump_cache[lump] = ptr;
  return ptr;
}

void* cache_lump_name(DoomState* state, str name, DoomPurgeTag tag) {
  return cache_lump_num(state, get_num_for_name(state, name), tag);
}

MaybeSize check_num_for_name(DoomState* state, str name) {
  for (size_t i = state->wad.num_lumps; i > 0; --i) {
    const char* raw_lump_name = state->wad.lump_info[i - 1].name;
    str lump_name;
    if (raw_lump_name[7] != '\0') {
      lump_name = str_ref_chars(raw_lump_name, 8);
    } else {
      lump_name = str_ref(raw_lump_name);
    }
    if (str_eq_ci(lump_name, name)) {
      return MAYBE_SOME(MaybeSize, i - 1);
    }
  }

  return MAYBE_NONE(MaybeSize);
}

size_t get_num_for_name(DoomState* state, str name) {
  MaybeSize i = check_num_for_name(state, name);
  if (i.is_present) {
    return i.value;
  }

  doom_error("get_num_for_name: %s not found", str_ptr(name));
}

size_t lump_length(DoomState* state, size_t lump) {
  if (lump >= state->wad.num_lumps) {
    doom_error("lump_length: Lump %zu out of range", lump);
  }

  return state->wad.lump_info[lump].size;
}

void read_lump(DoomState* state, size_t lump, void* dest) {
  if (lump >= state->wad.num_lumps) {
    doom_error("read_lump: Lump %zu out of range", lump);
  }

  DoomLumpInfo* lump_info = &state->wad.lump_info[lump];
  if (lump_info->handle == NULL) {
    doom_error("read_lump: Lump %zu not opened", lump);
  }

  (void)fseek(lump_info->handle, lump_info->position, SEEK_SET);
  size_t c = fread(dest, 1, lump_info->size, lump_info->handle);
  if (c != lump_info->size) {
    doom_error("read_lump: Failed to read lump %zu", lump);
  }
}

static void l_add_file(DoomState* state, str filename) {
  assert(*str_end(filename) == '\0');
  FILE* handle = fopen(str_ptr(filename), "rb");
  if (handle == NULL) {
    printf(" Failed to open file %s\n", str_ptr(filename));
    return;
  }

  printf(" Adding file %s\n", str_ptr(filename));
  size_t start_lump = state->wad.num_lumps;

  str filename_ext = str_ref(str_end(filename) - 4);
  DoomFileLump* file_info;
  bool free_file_info = false;
  DoomFileLump single_info;
  if (!str_has_suffix_ci(filename, str_lit(".wad"))) {
    // single lump
    file_info = &single_info;
    single_info.file_pos = 0;
    single_info.size = (int32_t)file_length(handle);
    l_extract_file_base(filename, single_info.name);
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
