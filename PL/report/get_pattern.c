#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "get_pattern.h"

int load_txt_to_8x8(const char filename[], int (**out)[8][8], size_t *out_n) {
    int *flat;
    int ret = load_txt(filename, 8, 8, &flat, out_n);
    if (ret != 0) {
        return ret;
    }
    *out = (int (*)[8][8])flat;
    return 0;
}

int load_txt(const char *filename, size_t h, size_t w, int **out, size_t *out_n) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;

    size_t cap = 1024, len = 0;
    int *buf = malloc(cap * sizeof(int));
    if (!buf) { fclose(fp); return -2; }

    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '0' || ch == '1') {
            if (len == cap) {
                cap *= 2;
                int *tmp = realloc(buf, cap * sizeof(int));
                if (!tmp) { free(buf); fclose(fp); return -2; }
                buf = tmp;
            }
            buf[len++] = ch - '0';
        }
    }
    fclose(fp);

    size_t cell = h * w;
    size_t n = (len + cell - 1) / cell;
    int *arr = calloc(n * cell, sizeof(int));
    if (!arr) { free(buf); return -2; }

    for (size_t i = 0; i < len; i++) arr[i] = buf[i];
    free(buf);

    *out = arr;
    *out_n = n;
    return 0;
}

void free_memory(void *arr) {
    free(arr);
}
