#ifndef FONTLIB_H
#define FONTLIB_H

#include <stddef.h>

void printx(int flag);
void printxs(int flag, const char str[]);

enum {
  PATTERN_DEFAULT_H = 8,
  PATTERN_DEFAULT_W = 8,
  PATTERN_SCALE_NORMAL = 1,
  PATTERN_SCALE_DOUBLE = 2
};

typedef enum { ROTATE_0, ROTATE_90, ROTATE_180, ROTATE_270 } PatternRotation;

typedef enum {
  PATTERN_EVENT_END,
  PATTERN_EVENT_CELL,
  PATTERN_EVENT_NEWLINE
} PatternEvent;

typedef struct {
  size_t count;
  size_t h;
  size_t w;
  void* pattern;
  const char* string;
  unsigned scale_w;
  unsigned scale_h;
  PatternRotation rotation;
} PatternView;

PatternView new_pattern_view(size_t count, void* pattern);
PatternView new_pattern_view_doublescale(size_t count, void* pattern);
void print_as_pattern(const PatternView* view);

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

typedef struct {
  size_t ch;
  size_t src_x;
  size_t src_y;
  PatternEvent kind;
} PatternStep;

typedef struct {
  const PatternView* view;
  size_t ch;
  size_t out_w;
  size_t out_h;
  size_t x;
  size_t y;
} PatternIterator;

PatternIterator new_iter(const PatternView* view);
PatternEvent pattern_iterator_next(PatternIterator* it, PatternStep* step);
PatternEvent pattern_iterator_next_h(PatternIterator* it, PatternStep* step);

int load_txt(const char* filename, size_t h, size_t w, int** out,
             size_t* out_n);
int load_txt_to_8x8(const char filename[], int (**out)[8][8], size_t* out_n);
void free_memory(void* arr);

#endif
