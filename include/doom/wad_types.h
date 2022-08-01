#pragma once

#include <stdint.h>
#include <stdio.h>

typedef struct {
  char identification[4];
  int32_t num_lumps;
  int32_t info_table_ofs;
} DoomWadInfo;

typedef struct {
  int32_t file_pos;
  int32_t size;
  char name[8];
} DoomFileLump;

typedef struct {
  char name[8];
  FILE* handle;
  int32_t position;
  int32_t size;
} DoomLumpInfo;
