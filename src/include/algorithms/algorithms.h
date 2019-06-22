#ifndef ALGORITHMS_H_
#define ALGORITHMS_H_

#include <mod_matrix/mod_matrix.h>

/**
 * @brief Performs the Gauss elimination algorithm
 * over a copy of matrix
 *
 * @param matrix
 * @return a copy of matrix in reduced row echelon form
 */
matrix_t*
algorithms_gauss_row_reduction(const matrix_t* matrix);

matrix_t * inverse(matrix_t * m);

matrix_t * projection(matrix_t * m);

uint32_t rank(matrix_t * m);

#endif // ALGORITHMS_H_
