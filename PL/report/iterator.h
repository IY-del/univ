#ifndef PATTERN_ITERATOR_H
#define PATTERN_ITERATOR_H

#include <stddef.h>
#include "print_pattern.h"

typedef enum {
    PATTERN_EVENT_END,
    PATTERN_EVENT_CELL,
    PATTERN_EVENT_NEWLINE
} PatternEvent;

typedef struct {
    size_t ch;
    size_t src_x;
    size_t src_y;
    PatternEvent kind;
} PatternStep;

typedef struct {
    const PatternView *view;
    size_t ch;
    size_t out_w;
    size_t out_h;
    size_t x;
    size_t y;
} PatternIterator;

PatternIterator new_iter(const PatternView *view);
PatternEvent pattern_iterator_next(PatternIterator *it, PatternStep *step);
PatternEvent pattern_iterator_next_h(PatternIterator *it, PatternStep *step);

#endif
