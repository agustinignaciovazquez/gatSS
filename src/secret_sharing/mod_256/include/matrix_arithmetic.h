//
// Created by Agustin Vazquez on 2019-06-10.
//

#ifndef GATSS_MATRIX_ARITHMETIC_H
#define GATSS_MATRIX_ARITHMETIC_H

#include "integer_arithmetic.h"
#include <stdint.h>
#include <inttypes.h>

typedef struct
{
    uint32_t rows;
    uint32_t cols;
    uint32_t * * values;
} matrix;

matrix* matrix_create_new(uint32_t rows, uint32_t cols);

void matrix_free(matrix* m);

matrix* matrix_sum(matrix* a, matrix* m2);

matrix* matrix_subtract(matrix* m1, matrix* m2);

matrix* matrix_multiply(matrix* a, matrix* m2);

matrix* matrixranspose(matrix* m);

matrix * matrix_create_copy(matrix * m);

void matrix_copy(matrix * from, matrix * to);

matrix * matrix_merge(matrix * a, matrix * b);

matrix * matrix_identity(uint32_t rows, uint32_t cols);

void matrix_print(matrix * matrix);

#endif //GATSS_MATRIX_ARITHMETIC_H
