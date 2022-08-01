#pragma once

#include "doom/state.h"
#include "doom/zone_types.h"

void zone_init(DoomState* state);
void* zone_malloc(DoomState* state, size_t size, DoomPurgeTag tag, void* user);
void zone_free(DoomState* state, void* ptr);
void zone_free_tags(
    DoomState* state, DoomPurgeTag low_tag, DoomPurgeTag high_tag);
void zone_dump_heap(
    DoomState* state, DoomPurgeTag low_tag, DoomPurgeTag high_tag);
void zone_file_dump_heap(DoomState* state, FILE* fp);
void zone_check_heap(DoomState* state);
void zone_change_tag(void* ptr, DoomPurgeTag tag);
size_t zone_free_memory(DoomState* state);
