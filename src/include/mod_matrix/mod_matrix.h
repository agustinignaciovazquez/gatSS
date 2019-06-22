#ifndef MOD_MATRIX_H_08b80052e752c78bea7877589436359b
#define MOD_MATRIX_H_08b80052e752c78bea7877589436359b

#include <mod/mod.h>
#include <stdint.h>
#include <inttypes.h>

typedef struct
{
  uint32_t rows;
  uint32_t cols;
  uint32_t * * values;
} matrix_t;

matrix_t* mod_matrix_new(uint32_t rows, uint32_t cols);

void mod_matrix_free(matrix_t* m);

matrix_t* mod_matrix_sum(matrix_t* a, matrix_t* m2);

matrix_t* mod_matrix_sub(matrix_t* m1, matrix_t* m2);

matrix_t* mod_matrix_mul(matrix_t* a, matrix_t* m2);

matrix_t* mod_matrix_transpose(matrix_t* m);

matrix_t * matrix_create_copy(matrix_t * m);

void matrix_copy(matrix_t * from, matrix_t * to);

matrix_t * merge(matrix_t * a, matrix_t * b);

matrix_t * ones(uint32_t rows, uint32_t cols);

void print_matrix(matrix_t * matrix);

#endif // MOD_MATRIX_H_08b80052e752c78bea7877589436359b
