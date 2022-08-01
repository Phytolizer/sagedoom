#pragma once

#include "doom/state.h"
#include "doom/wad_types.h"
#include "doom/zone.h"
#include "util/maybe.h"

#include <str.h>

void wad_init_multiple_files(DoomState* state);
void* cache_lump_num(DoomState* state, size_t lump, DoomPurgeTag tag);
void* cache_lump_name(DoomState* state, str name, DoomPurgeTag tag);
MaybeSize check_num_for_name(DoomState* state, str name);
size_t get_num_for_name(DoomState* state, str name);
size_t lump_length(DoomState* state, size_t lump);
void read_lump(DoomState* state, size_t lump, void* dest);
