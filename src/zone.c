#include "doom/zone.h"

#include "doom/error.h"
#include "doom/types.h"

#include <stdlib.h>

#define IS_MARKED_FREE(P) ((P)->user == NULL)
#define MARK_FREE(P) ((P)->user = NULL)
#define ALIGN_BY(Align, Value) (((Value) + (Align)-1) & ~((Align)-1))

enum {
  ZONE_ID = 0x1D4A11,
  ZONE_BASE_SIZE = 6 * 1024 * 1024,
  MIN_FRAGMENT = 64,
};

static void l_clear_zone(MemoryZone* zone);

void zone_init(DoomState* state) {
  state->main_zone = malloc(ZONE_BASE_SIZE);
  state->main_zone->size = ZONE_BASE_SIZE;

  l_clear_zone(state->main_zone);
}

void* zone_malloc(DoomState* state, size_t size, DoomPurgeTag tag, void* user) {
  size = ALIGN_BY(4, size);
  size += sizeof(MemoryBlock);
  MemoryBlock* base = state->main_zone->rover;
  if (IS_MARKED_FREE(base->prev)) {
    base = base->prev;
  }

  MemoryBlock* rover = base;
  MemoryBlock* start = base->prev;

  do {
    if (rover == start) {
      doom_error("zone_malloc: Failed allocating %zu bytes", size);
    }

    if (!IS_MARKED_FREE(rover)) {
      if (rover->tag < DOOM_PURGE_TAG_PURGE_LEVEL) {
        base = rover->next;
        rover = rover->next;
      } else {
        base = base->prev;
        zone_free(state, (Byte*)rover + sizeof(MemoryBlock));
        base = base->next;
        rover = base->next;
      }
    } else {
      rover = rover->next;
    }
  } while (!IS_MARKED_FREE(base) || base->size < size);

  size_t extra = base->size - size;
  if (extra > MIN_FRAGMENT) {
    MemoryBlock* new_block = (MemoryBlock*)((Byte*)base + size);
    new_block->size = extra;

    MARK_FREE(new_block);
    new_block->tag = 0;
    new_block->prev = base;
    new_block->next = base->next;
    new_block->next->prev = new_block;

    base->next = new_block;
    base->size = size;
  }

  if (user != NULL) {
    base->user = user;
    *(void**)user = ((Byte*)base + sizeof(MemoryBlock));
  } else {
    if (tag >= DOOM_PURGE_TAG_PURGE_LEVEL) {
      doom_error("zone_malloc: An owner is required for purgable blocks");
    }

    base->user = (void*)2;
  }

  base->tag = tag;
  state->main_zone->rover = base->next;
  base->id = ZONE_ID;

  return (Byte*)base + sizeof(MemoryBlock);
}

void zone_free(DoomState* state, void* ptr) {
  MemoryBlock* block = (MemoryBlock*)((Byte*)ptr - sizeof(MemoryBlock));
  if (block->id != ZONE_ID) {
    doom_error(
        "zone_free: Freed a pointer that was not allocated by zone_malloc");
  }

  if (block->user > (void**)0x100) {
    // values <= 0x100 are not pointers (probably)
    *block->user = NULL;
  }

  MARK_FREE(block);
  block->tag = 0;
  block->id = 0;

  MemoryBlock* other = block->prev;
  if (IS_MARKED_FREE(other)) {
    other->size += block->size;
    other->next = block->next;
    other->next->prev = other;

    if (block == state->main_zone->rover) {
      state->main_zone->rover = other;
    }

    block = other;
  }

  other = block->next;
  if (IS_MARKED_FREE(other)) {
    block->size += other->size;
    block->next = other->next;
    block->next->prev = block;

    if (other == state->main_zone->rover) {
      state->main_zone->rover = block;
    }
  }
}

void zone_free_tags(
    DoomState* state, DoomPurgeTag low_tag, DoomPurgeTag high_tag) {
  MemoryBlock* block = state->main_zone->block_list.next;
  while (block != &state->main_zone->block_list) {
    MemoryBlock* next = block->next;
    if (IS_MARKED_FREE(block)) {
      block = next;
      continue;
    }

    if (block->tag >= low_tag && block->tag <= high_tag) {
      zone_free(state, (Byte*)block + sizeof(MemoryBlock));
    }
    block = next;
  }
}

void zone_dump_heap(
    DoomState* state, DoomPurgeTag low_tag, DoomPurgeTag high_tag) {
  printf("zone size: %zu, location: %p\n",
      state->main_zone->size,
      (void*)state->main_zone);
  printf("tag range: %d to %d\n", low_tag, high_tag);

  for (MemoryBlock* block = state->main_zone->block_list.next;;
       block = block->next) {
    if (block->tag >= low_tag && block->tag <= high_tag) {
      printf("block: %p    size: %7zu    user: %p    tag: %3d\n",
          (void*)block,
          block->size,
          (void*)block->user,
          block->tag);
    }

    if (block->next == &state->main_zone->block_list) {
      break;
    }

    if ((Byte*)block + block->size != (Byte*)block->next) {
      printf("ERROR: block size does not touch the next block\n");
    }

    if (block->next->prev != block) {
      printf("ERROR: next block doesn't have proper back link\n");
    }

    if (IS_MARKED_FREE(block) && IS_MARKED_FREE(block->next)) {
      printf("ERROR: two consecutive free blocks\n");
    }
  }
}

void zone_file_dump_heap(DoomState* state, FILE* fp) {
  (void)fprintf(fp,
      "zone size: %zu  location: %p\n",
      state->main_zone->size,
      (void*)state->main_zone);

  for (MemoryBlock* block = state->main_zone->block_list.next;;
       block = block->next) {
    (void)fprintf(fp,
        "block: %p    size: %7zu    user: %p    tag: %3d\n",
        (void*)block,
        block->size,
        (void*)block->user,
        block->tag);

    if (block->next == &state->main_zone->block_list) {
      break;
    }

    if ((Byte*)block + block->size != (Byte*)block->next) {
      (void)fprintf(fp, "ERROR: block size does not touch the next block\n");
    }

    if (block->next->prev != block) {
      (void)fprintf(fp, "ERROR: next block doesn't have proper back link\n");
    }

    if (IS_MARKED_FREE(block) && IS_MARKED_FREE(block->next)) {
      (void)fprintf(fp, "ERROR: two consecutive free blocks\n");
    }
  }
}

void zone_check_heap(DoomState* state) {
  for (MemoryBlock* block = state->main_zone->block_list.next;;
       block = block->next) {
    if (block->next == &state->main_zone->block_list) {
      break;
    }

    if ((Byte*)block + block->size != (Byte*)block->next) {
      doom_error("zone_check_heap: Block size does not touch the next block");
    }
    if (block->next->prev != block) {
      doom_error("zone_check_heap: Next block doesn't have proper back link");
    }
    if (IS_MARKED_FREE(block) && IS_MARKED_FREE(block->next)) {
      doom_error("zone_check_heap: Two consecutive free blocks");
    }
  }
}

void zone_change_tag(void* ptr, DoomPurgeTag tag) {
  MemoryBlock* block = (MemoryBlock*)((Byte*)ptr - sizeof(MemoryBlock));
  if (block->id != ZONE_ID) {
    doom_error("zone_change_tag: Freed a pointer that was not allocated by "
               "zone_malloc");
  }

  if (tag >= DOOM_PURGE_TAG_PURGE_LEVEL && (size_t)block->user < 0x100) {
    doom_error("zone_change_tag: An owner is required for purgable blocks");
  }

  block->tag = tag;
}

size_t zone_free_memory(DoomState* state) {
  size_t free_amt = 0;
  for (MemoryBlock* block = state->main_zone->block_list.next;
       block != &state->main_zone->block_list;
       block = block->next) {
    if (IS_MARKED_FREE(block) || block->tag >= DOOM_PURGE_TAG_PURGE_LEVEL) {
      free_amt += block->size;
    }
  }
  return free_amt;
}

static void l_clear_zone(MemoryZone* zone) {
  MemoryBlock* free_block = (MemoryBlock*)((Byte*)zone + sizeof(MemoryZone));
  zone->block_list.next = free_block;
  zone->block_list.prev = free_block;

  zone->block_list.user = (void**)zone;
  zone->block_list.tag = DOOM_PURGE_TAG_STATIC;
  zone->rover = free_block;

  free_block->next = &zone->block_list;
  free_block->prev = &zone->block_list;
  MARK_FREE(free_block);
  free_block->size = zone->size - sizeof(MemoryZone);
}
