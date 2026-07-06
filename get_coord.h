#ifndef GET_COORD_H
#define GET_COORD_H

#include <stddef.h>
#include "print_pattern.h"

typedef struct {
    size_t x;
    size_t y;
} PatternCoord;

size_t pattern_output_width(const PatternView *view);
size_t pattern_output_height(const PatternView *view);
size_t pattern_scaled_width(const PatternView *view);
size_t pattern_scaled_height(const PatternView *view);

PatternCoord resolve_coord(const PatternView *view, size_t out_x, size_t out_y);

#endif
