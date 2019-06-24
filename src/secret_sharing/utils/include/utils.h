#ifndef UTILS_H
#define UTILS_H

#include "bmp.h"
#include "matrix_operations.h"
#include <stdlib.h>

int64_t rand_seed; //Global seed

uint8_t rand_next_char(void);

void rand_set_seed(int64_t s);

uint32_t int_pow(uint32_t x, uint32_t y);

void sm_from_bmp(matrix *sm, BMPImage * s, uint32_t base);

void sm_merge_matrix(matrix * m, matrix * sm, uint32_t base);

#endif // UTILS_H
