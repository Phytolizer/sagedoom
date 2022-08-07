#pragma once

typedef struct {
  int width;
  int height;
  int left_offset;
  int top_offset;
  int column_offset[8];
} Patch;
