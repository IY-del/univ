#include <stdio.h>
#include "print_pattern.h"
#include "fontlib.h"
#include "iterator.h"
#include "get_coord.h"

PatternView new_pattern_view(size_t count, void *pattern) {
    PatternView view = {
        .count    = count,
        .h        = PATTERN_DEFAULT_H,
        .w        = PATTERN_DEFAULT_W,
        .pattern  = pattern,
        .string   = "X",
        .scale_w  = PATTERN_SCALE_NORMAL,
        .scale_h  = PATTERN_SCALE_NORMAL,
        .rotation = ROTATE_0
    };

    return view;
}

PatternView new_pattern_view_doublescale(size_t count, void *pattern) {
    PatternView view = new_pattern_view(count, pattern);
    view.scale_w = PATTERN_SCALE_DOUBLE;
    view.scale_h = PATTERN_SCALE_DOUBLE;
    return view;
}

void print_as_pattern(const PatternView *view) {
    int (*pattern)[view->h][view->w] = view->pattern;
    PatternIterator it = new_iter(view);
    PatternStep step;
    while (pattern_iterator_next(&it, &step)) {
        switch (step.kind) {
            case PATTERN_EVENT_CELL:
                printxs(pattern[step.ch][step.src_y][step.src_x], view->string);
                break;
            case PATTERN_EVENT_NEWLINE:
                putchar('\n');
                break;
            case PATTERN_EVENT_END:
                break;
        }
    }
    putchar('\n');
}
