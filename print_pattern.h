#ifndef PRINT_PATTERN_H
#define PRINT_PATTERN_H

#include <stddef.h>

enum {
    PATTERN_DEFAULT_H    = 8,
    PATTERN_DEFAULT_W    = 8,
    PATTERN_SCALE_NORMAL = 1,
    PATTERN_SCALE_DOUBLE = 2
};

typedef enum {
    ROTATE_0,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270
} PatternRotation;

typedef struct {
    size_t count;
    size_t h;
    size_t w;
    void *pattern;
    const char *string;
    unsigned scale_w;
    unsigned scale_h;
    PatternRotation rotation;
} PatternView;

PatternView new_pattern_view(size_t count, void *pattern);
PatternView new_pattern_view_doublescale(size_t count, void *pattern);
void print_as_pattern(const PatternView *view);

#endif