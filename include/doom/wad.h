#pragma once

#include "doom/state.h"
#include "doom/zone.h"

void* cache_lump_name(DoomState* state, const char* name, DoomPurgeTag tag);
