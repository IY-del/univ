#include <stdio.h>
#include "print_pattern.h"
#include "get_pattern.h"

int main() {
    int (*patterns)[8][8];
    size_t n;

    int ret = load_txt_to_padded_8x8("font_data02.txt", &patterns, &n);
    if (ret != 0) {
        fprintf(stderr, "failed to load font data: %d\n", ret);
        return 1;
    }

    PatternView view = new_pattern_view_doublescale(n, patterns);
    print_as_pattern(&view);

    free_padded_8x8(patterns);
}
