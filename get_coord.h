#ifndef GET_COORD_H
#define GET_COORD_H

#include <stddef.h>

#include "print_pattern.h"

typedef struct {
  size_t x;
  size_t y;
} PatternCoord;

typedef struct {
  size_t width;
  size_t height;
} PatternSize;

PatternSize pattern_scaled_size(const PatternView* view);
PatternCoord resolve_coord(const PatternView* view, size_t out_x, size_t out_y);

#endif
