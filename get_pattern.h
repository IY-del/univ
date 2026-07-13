int load_txt_to_8x8(const char filename[], int (**out)[8][8], size_t *out_n);
int load_txt(const char *filename, size_t h, size_t w, int **out, size_t *out_n);
void free_memory(int *arr);