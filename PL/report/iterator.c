#include <stdio.h>
#include "iterator.h"
#include "get_coord.h"

PatternIterator new_iter(const PatternView *view) {
    PatternIterator it = {
        .view = view,
        .out_w = pattern_scaled_width(view),
        .out_h = pattern_scaled_height(view),
        .x = 0,
        .y = 0,
        .ch = 0,
    };
    
    return it;
}

PatternEvent pattern_iterator_next(PatternIterator *it, PatternStep *step) {
    if (it->x >= it->out_w) {
        it->x = 0;
        it->y++;
        step->kind = PATTERN_EVENT_NEWLINE;
        if (it->y >= it->out_h) {
            it->y = 0;
            it->ch++;
        }

        if (it->ch >= it->view->count) {
            step->kind = PATTERN_EVENT_END;
        }
        return step->kind;
    }
    PatternCoord src = resolve_coord(it->view, it->x, it->y);
    step->ch = it->ch;
    step->src_x = src.x;
    step->src_y = src.y;
    step->kind = PATTERN_EVENT_CELL;
    it->x++;
    return step->kind;
}

PatternEvent pattern_iterator_next_h(PatternIterator *it, PatternStep *step) {
    if (it->x >= it->out_w) {
        it->x = 0;
        it->ch++;
    }
    if (it->ch >= it->view->count) {
        it->ch = 0;
        it->y++;
        step->kind = PATTERN_EVENT_NEWLINE;
        if (it->y >= it->out_h) {
            step->kind = PATTERN_EVENT_END;
        }
        return step->kind;
    }
    PatternCoord src = resolve_coord(it->view, it->x, it->y);
    step->ch = it->ch;
    step->src_x = src.x;
    step->src_y = src.y;
    step->kind = PATTERN_EVENT_CELL;
    it->x++;
    return step->kind;
}
