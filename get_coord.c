#include "get_coord.h"

static PatternCoord rotate_coord(const PatternView *view, size_t x, size_t y) {
    PatternCoord coord = {0, 0};
    switch (view->rotation) {
        case ROTATE_0:
            coord.x = x;
            coord.y = y;
            break;
        case ROTATE_90:
            coord.x = y;
            coord.y = view->h - 1 - x;
            break;
        case ROTATE_180:
            coord.x = view->w - 1 - x;
            coord.y = view->h - 1 - y;
            break;
        case ROTATE_270:
            coord.x = view->w - 1 - y;
            coord.y = x;
            break;
    }

    return coord;
}

size_t pattern_output_width(const PatternView *view) {
    if (view->rotation == ROTATE_90 || view->rotation == ROTATE_270) {
        return view->h;
    }

    return view->w;
}

size_t pattern_output_height(const PatternView *view) {
    if (view->rotation == ROTATE_90 || view->rotation == ROTATE_270) {
        return view->w;
    }
    
    return view->h;
}

size_t pattern_scaled_width(const PatternView *view) {
    return pattern_output_width(view) * view->scale_w;
}

size_t pattern_scaled_height(const PatternView *view) {
    return pattern_output_height(view) * view->scale_h;
}

PatternCoord resolve_coord(const PatternView *view, size_t out_x, size_t out_y) {
    size_t logical_x = out_x / view->scale_w;
    size_t logical_y = out_y / view->scale_h;
    return rotate_coord(view, logical_x, logical_y);
}
