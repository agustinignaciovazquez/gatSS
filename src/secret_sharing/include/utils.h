#ifndef UTILS_H
#define UTILS_H

#include <bmp/bmp.h>
#include <stdlib.h>
#include "../mod_256/include/matrix_operations.h"
int64_t rand_seed;

uint8_t rand_next_char(void);

void rand_set_seed(int64_t s);

int check_shadow_sizes(BMPImage * secret,  BMPImage ** shadows, size_t len, int n, int k);

void get_sub_matrix_from_image(matrix *s, BMPImage *secret, int base_col, int base_row);

void get_sub_matrix_into_image(matrix *s, BMPImage *secret, int base_col, int base_row);

void get_sub_matrix_from_matrix(matrix *s, matrix *secret, int base_col, int base_row);

void create_random_A_matrix(matrix *A);

void create_random_x_vectors(matrix **X, int n, int k);

void generate_G_matrix(matrix **G, matrix *R, int n, int k);

void merge_sub_matrix_into_matrix(matrix * m, matrix *sub_matrix, int base_row, int base_col);

void hide_shares(matrix **shares, BMPImage **shadows, int n);

uint32_t int_pow(uint32_t x, uint32_t y);

void save_rw_to_image(BMPImage * s, matrix * rw);

BMPImage * build_image(BMPImage * base);

int printv(const char *fmt, ...);

int check_bmp_sizes(BMPImage ** bmp_list, int len);

size_t shadow_size_for(uint32_t image_size, int n, int k);

int check_shadow_sizes(BMPImage * secret,  BMPImage ** shadows, size_t len, int n, int k);

#endif // UTILS_H
