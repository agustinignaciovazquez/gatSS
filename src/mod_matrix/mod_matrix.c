#include <mod_matrix/mod_matrix.h>
#include <algorithms/algorithms.h>
#include <mod_matrix/mod_matrix_macros.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <logger/logger.h>
/**
 * @brief Applies op between elements of the
 * matrices: m1[i,j] op m2[i,j] ; 0<i<rows, 0<j<cols.
 * Matrices sizes must match!
 *
 * @param m1 Matrix 1
 * @param m2 Matrix 2
 * @param op Operator
 * @return matrix_t*
 */
static inline matrix_t*
mod_matrix_binary_elem_op_applier(matrix_t* m1,
                                  matrix_t* m2,
                                  uint32_t (*op)(uint32_t, uint32_t))
{
  if (m1->cols != m2->cols || m1->rows != m2->rows) {
    LOG_ERROR("Matrix sizes must match!");
    exit(EXIT_FAILURE);
  }

  matrix_t* ans = NEW_MATRIX(m1->cols, m1->rows);
  ans->rows     = m1->rows;
  ans->cols     = m1->cols;

  uint32_t* ans_p = ans->values;
  uint32_t* m1_p  = m1->values;
  uint32_t* m2_p  = m2->values;
  uint32_t i      = ans->cols * ans->rows;

  while (i--) {
    *(ans_p++) = op(*(m1_p++), *(m2_p++));
  }

  return ans;
}

matrix_t*
mod_matrix_sum(matrix_t* m1, matrix_t* m2)
{
  return mod_matrix_binary_elem_op_applier(m1, m2, mod_sum);
}

matrix_t*
mod_matrix_sub(matrix_t* m1, matrix_t* m2)
{
  return mod_matrix_binary_elem_op_applier(m1, m2, mod_sub);
}

matrix_t*
mod_matrix_transpose(matrix_t* m)
{
  matrix_t* ans = NEW_MATRIX(m->cols, m->rows);
  ans->rows     = m->cols;
  ans->cols     = m->rows;

  for (uint32_t row = 0; row < m->rows; row++) {
    for (uint32_t col = 0; col < m->cols; col++) {
      ans->values[col * ans->cols + row] = m->values[row * m->cols + col];
    }
  }

  return ans;
}

matrix_t*
mod_matrix_mul(matrix_t* m1, matrix_t* m2)
{
  if (m1->cols != m2->rows) {
    LOG_ERROR("Inner matrix dimensions must match!");
    exit(EXIT_FAILURE);
  }

  matrix_t* ans = NEW_MATRIX(m1->rows, m2->cols);
  ans->rows     = m1->rows;
  ans->cols     = m2->cols;

  /** Transpose m2 to use memory secuentially */
  matrix_t* m2_tr = mod_matrix_transpose(m2);

  uint32_t inner_dim = m1->cols;
  uint32_t sum;

  for (uint32_t row = 0; row < ans->rows; row++) {
    for (uint32_t col = 0; col < ans->cols; col++) {
      sum = 0;
      for (uint32_t k = 0; k < inner_dim; k++) {
        sum = mod_sum(sum,
                      mod_mul(m1->values[row * m1->cols + k],
                              m2_tr->values[col * m2_tr->cols + k]));
      }
      ans->values[row * ans->cols + col] = sum;
    }
  }

  /** Free m2_tr as it won't be used */
  free(m2_tr);

  return ans;
}

matrix_t*
mod_matrix_mul_esc(uint32_t e, matrix_t* m)
{
    matrix_t* ans = NEW_MATRIX(m->cols, m->rows);
    ans->rows     = m->cols;
    ans->cols     = m->rows;
    int i, j = 0;
    for(i = 0; i < m->rows; i++) {
        for (j = 0; j < m->cols; j++) {
            ans->values[i*ans->cols + j] = mod_mul(m->values[i*m->cols + j], e);
        }
    }
    return ans;
}

void print_matrix(matrix_t * matrix) {

    char * temp_line = malloc(3*(matrix->cols*sizeof(char) + 1));

    for(int i = 0; i < matrix->rows; i++) {
        for(int j = 0; j < matrix->cols; j++) {
            sprintf(temp_line + j*4, "%3d%s", ELEM(matrix, i, j), j!=matrix->cols-1 ? " " : "");
        }
        LOG_INFO("%s\n", temp_line);
    }
    free(temp_line);
}

matrix_t * ones(uint32_t rows, uint32_t cols) {
    matrix_t * m = NEW_MATRIX(rows, cols);
    m->rows = rows;
    m->cols = cols;

    for(int i = 0; i < m->rows; i++) {
        for(int j = 0; j < m->cols; j++) {
            if(i == j)
                 m->values[i*cols + j] = 1;
            else
                m->values[i*cols + j] = 0;
        }
    }
    return m;
}

matrix_t * merge(matrix_t * m1, matrix_t * m2) {
    matrix_t * m = NEW_MATRIX(m1->rows, m1->cols + m2->cols);
    m->rows = m1->rows;
    m->cols = m1->cols + m2->cols;
    uint32_t * aux_m = m->values;
    for(int i = 0; i < m1->rows; i++) {
        memcpy(aux_m, m1->values + i * m1->cols, m1->cols*sizeof(*m1->values));
        aux_m += m1->cols;
        memcpy(aux_m, m2->values + i * m2->cols, m2->cols*sizeof(*m2->values));
        aux_m += m2->cols;

    }

    return m;
}

matrix_t*
mod_matrix_new(uint32_t rows, uint32_t cols) {
  matrix_t * m = malloc(sizeof(matrix_t) + cols * rows * sizeof(*m->values));
  m->rows = rows;
  m->cols = cols;

  return m;
}
