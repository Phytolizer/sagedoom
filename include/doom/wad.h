#pragma once

#include "doom/state.h"
#include "doom/wad_types.h"
#include "doom/zone.h"

void wad_init_multiple_files(DoomState* state, const char* const* filenames);
void* cache_lump_name(DoomState* state, const char* name, DoomPurgeTag tag);
