#ifndef MOD_MATRIX_MACROS_H_2143e512a57add6f2d061262d57fe60d
#define MOD_MATRIX_MACROS_H_2143e512a57add6f2d061262d57fe60d

#include <mod_matrix/mod_matrix.h>
#include <stdlib.h>

#define NEW_MATRIX(rows, cols)                                                 \
  (malloc(sizeof(matrix_t) + (rows) * (cols) * sizeof(uint32_t)))

#define ELEM(matrix_ptr, row, col)                                             \
  ((matrix_ptr)->values[(row) * (matrix_ptr)->cols + (col)])

#endif // MOD_MATRIX_MACROS_H_2143e512a57add6f2d061262d57fe60d
