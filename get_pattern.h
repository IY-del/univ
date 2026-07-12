int load_txt_to_padded_8x8(const char filename[], int (**out)[8][8], size_t *out_n);
void free_padded_8x8(int (*arr)[8][8]);