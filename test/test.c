#include <stdio.h>

#include "fontlib.h"
#include "get_coord.h"
#include "get_pattern.h"
#include "iterator.h"
#include "print_pattern.h"

/* fontlib のテスト ------------------------------------------------------- */
#ifdef TEST_FONTLIB
void test_fontlib(void) {
  printx(1);
  printx(0);
  printx(2);
  putchar('\n');

  printxs(1, "ABC");
  putchar('\n');

  printxs(0, "ABC");
  putchar('\n');
}
#endif

/* get_pattern のテスト --------------------------------------------------- */
#ifdef TEST_GET_PATTERN
void test_get_pattern(void) {
  int (*patterns)[8][8];
  size_t n;

  if (load_txt_to_8x8("../font_data01.txt", &patterns, &n) != 0) {
    fprintf(stderr, "load_txt_to_8x8 failed (../font_data01.txt)\n");
    return;
  }

  printf("n = %zu\n", n);

  if (n > 0) {
    for (size_t y = 0; y < 8; y++) {
      for (size_t x = 0; x < 8; x++) {
        printf("%d", patterns[0][y][x]);
      }
      putchar('\n');
    }
  }

  free_memory(patterns);
}
#endif

/* get_coord のテスト ----------------------------------------------------- */
#ifdef TEST_GET_COORD
static void report_coord(const char* label, const PatternView* view, size_t ox,
                         size_t oy) {
  PatternSize size = pattern_scaled_size(view);
  PatternCoord src = resolve_coord(view, ox, oy);
  printf("%-22s size=(w=%zu,h=%zu) resolve(%zu,%zu) -> (%zu,%zu)\n", label,
         size.width, size.height, ox, oy, src.x, src.y);
}

void test_get_coord(void) {
  int dummy[1][8][8] = {{{0}}};
  PatternView view = new_pattern_view(1, dummy);

  /* 非正方形にして w/h の入れ替わりを可視化する */
  view.w = 8;
  view.h = 4;

  /* 1. 回転なし（基準確認） */
  view.rotation = ROTATE_0;
  report_coord("rotate0", &view, 0, 0);
  report_coord("rotate0 corner", &view, 7, 3);

  /* 2. 90度回転（w/hが入れ替わるはず） */
  view.rotation = ROTATE_90;
  report_coord("rotate90", &view, 0, 0);
  report_coord("rotate90 corner", &view, 3, 7);

  /* 3. 180度回転（未検証だった経路） */
  view.rotation = ROTATE_180;
  report_coord("rotate180", &view, 0, 0);
  report_coord("rotate180 corner", &view, 7, 3);

  /* 4. 270度回転（未検証だった経路） */
  view.rotation = ROTATE_270;
  report_coord("rotate270", &view, 0, 0);
  report_coord("rotate270 corner", &view, 3, 7);

  /* 5. 非対称スケール（scale_wとscale_hを別々の値にして独立性を確認） */
  view.rotation = ROTATE_0;
  view.scale_w = 3;
  view.scale_h = 2;
  report_coord("scale_w3_h2", &view, 0, 0);

  /* 6. 回転90度 + 非対称スケールの組み合わせ */
  view.rotation = ROTATE_90;
  report_coord("rotate90_scale_w3_h2", &view, 0, 0);
}
#endif

/* iterator のテスト ------------------------------------------------------ */
#ifdef TEST_ITERATOR
static void run_next(const PatternView* view) {
  PatternIterator it = new_iter(view);
  PatternStep step;

  printf("pattern_iterator_next\n");
  while (pattern_iterator_next(&it, &step) != PATTERN_EVENT_END) {
    if (step.kind == PATTERN_EVENT_CELL) {
      printf("CELL ch=%zu src=(%zu,%zu)\n", step.ch, step.src_x, step.src_y);
    } else if (step.kind == PATTERN_EVENT_NEWLINE) {
      printf("NEWLINE\n");
    }
  }
  printf("END\n");
}

static void run_next_h(const PatternView* view) {
  PatternIterator it = new_iter(view);
  PatternStep step;

  printf("pattern_iterator_next_h\n");
  while (pattern_iterator_next_h(&it, &step) != PATTERN_EVENT_END) {
    if (step.kind == PATTERN_EVENT_CELL) {
      printf("CELL ch=%zu src=(%zu,%zu)\n", step.ch, step.src_x, step.src_y);
    } else if (step.kind == PATTERN_EVENT_NEWLINE) {
      printf("NEWLINE\n");
    }
  }
  printf("END\n");
}

void test_iterator(void) {
  int dummy[2][8][8] = {{{0}}};
  PatternView view = new_pattern_view(2, dummy);

  run_next(&view);
  putchar('\n');
  run_next_h(&view);
}
#endif

/* print_pattern のテスト ------------------------------------------------- */
#ifdef TEST_PRINT_PATTERN
void test_print_pattern(void) {
  int patterns[1][8][8] = {{{0, 0, 1, 1, 0, 0, 0, 0},
                            {0, 0, 1, 1, 0, 0, 0, 0},
                            {0, 0, 1, 1, 0, 0, 0, 0},
                            {1, 1, 1, 1, 1, 1, 0, 0},
                            {1, 1, 0, 0, 1, 1, 0, 0},
                            {1, 1, 0, 0, 1, 1, 0, 0},
                            {1, 1, 0, 0, 1, 1, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0}}};

  PatternView view = new_pattern_view(1, patterns);
  printf("normal: h=%zu w=%zu scale_w=%u scale_h=%u rotation=%d\n", view.h,
         view.w, view.scale_w, view.scale_h, view.rotation);
  print_as_pattern(&view);

  PatternView view2x = new_pattern_view_doublescale(1, patterns);
  printf("double: h=%zu w=%zu scale_w=%u scale_h=%u rotation=%d\n", view2x.h,
         view2x.w, view2x.scale_w, view2x.scale_h, view2x.rotation);
  print_as_pattern(&view2x);
}
#endif

/* main 相当のテスト ------------------------------------------------------ */
#ifdef TEST_MAIN_LIKE
void test_main_like(void) {
  int (*patterns)[8][8];
  size_t n;

  if (load_txt_to_8x8("../font_data01.txt", &patterns, &n) != 0) {
    fprintf(stderr, "failed to load ../font_data01.txt\n");
    return;
  }

  PatternView view = new_pattern_view(n, patterns);
  print_as_pattern(&view);

  free_memory(patterns);
}
#endif

/* エントリポイント ------------------------------------------------------- */
int main(void) {
#ifdef TEST_FONTLIB
  test_fontlib();
#elif defined(TEST_GET_PATTERN)
  test_get_pattern();
#elif defined(TEST_GET_COORD)
  test_get_coord();
#elif defined(TEST_ITERATOR)
  test_iterator();
#elif defined(TEST_PRINT_PATTERN)
  test_print_pattern();
#elif defined(TEST_MAIN_LIKE)
  test_main_like();
#else
  printf("No test macro defined.\n");
#endif
  return 0;
}
