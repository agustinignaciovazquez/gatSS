#ifndef UTILS_H
#define UTILS_H

#include "bmp.h"
#include "matrix_operations.h"
#include <stdlib.h>

int64_t rand_seed; //Global seed

uint8_t rand_next_char(void);

void rand_set_seed(int64_t s);
uint8_t safe_next_char(void); //TODO VER SI ES NECESARIO ESTO

uint32_t int_pow(uint32_t x, uint32_t y);

size_t shadow_size_for(uint32_t image_size, uint32_t n, uint32_t k);

void get_sub_matrix_from_image(matrix *sm, BMPImage * s, uint32_t base);

void get_sub_matrix_into_image(matrix *sm, BMPImage * s, uint32_t base);

void get_sub_matrix_into_matrix(matrix * m, matrix * sm, uint32_t base);

void get_sub_matrix_from_matrix(matrix *sm, matrix *s, uint32_t base);

uint32_t check_shadow_sizes(BMPImage * secret,  BMPImage ** shadows, size_t len, uint32_t n, uint32_t k);

#endif // UTILS_H
