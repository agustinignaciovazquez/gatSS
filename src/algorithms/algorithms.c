#include <algorithms/algorithms.h>
#include <mod/mod.h>
#include <mod_matrix/mod_matrix.h>
#include <mod_matrix/mod_matrix_macros.h>
#include <stdio.h>
#include <string.h>

static inline uint8_t find_non_zero_col(const matrix_t *matrix,
                  uint32_t start,
                  uint32_t col,
                  uint32_t *r){
  for (uint32_t row = start; row < matrix->rows; row++) {
    if (matrix->values[row][col] != 0) {
      *r = row;
      return 1;
    }
  }
  return 0;
}

static inline void swap_rows(matrix_t *m, uint32_t row1, uint32_t row2){
  uint32_t * temp;
  temp = m->values[row1];
  m->values[row1] = m->values[row2];
  m->values[row2] = temp;
}

matrix_t* algorithms_gauss_row_reduction(const matrix_t* matrix){
  matrix_t* m = matrix_create_copy(matrix);

  uint32_t pivot_row   = 0;
  uint32_t pivot_col   = 0;
  uint32_t pivot_value = 0;

  while (pivot_row < m->rows && pivot_col < m->cols) {

    uint32_t temp    = 0;

    /** find the first non-zero remaining in column j */
    if (find_non_zero_col(m, pivot_row, pivot_col, &temp)) {
      /** an element was found */
      pivot_value = m->values[temp][pivot_col];
    } else {
      pivot_value = 0;
    }

    if (pivot_value == 0) {

      /** The column is negligible, zero it out. */
      for (uint32_t row = pivot_row; row < m->rows; row++) {
        m->values[row][pivot_col] = 0;
      }
      pivot_col++;

    } else {

      /** Swap i-th and k-th rows. */
        swap_rows(m, pivot_row, temp);

      /** Divide the pivot row by the pivot element. */
      for (uint32_t col = pivot_col; col < m->cols; col++) {
        m->values[pivot_row][col] = mod_div(m->values[pivot_row][col], pivot_value);
      }

      /** Subtract multiples of the pivot row from all the other rows. */
      for (uint32_t row = (pivot_row == 0 ? 1 : 0); row < m->rows;
           row += (row == pivot_row - 1 ? 2 : 1)) {
        uint32_t pivot_temp = m->values[row][pivot_col];
        for (uint32_t col = pivot_col; col < m->cols; col++) {
          m->values[row][col] = mod_sub(m->values[row][col], mod_mul(pivot_temp, m->values[pivot_row][col]));
        }
      }

      pivot_row++;
      pivot_col++;
    }
  }

  return m;
}

uint32_t rank(matrix_t * m) {
    matrix_t * gm = algorithms_gauss_row_reduction(mod_matrix_transpose(m));
    uint32_t r = 0,c = 0;

    for (uint32_t row = 0; row < gm->rows; row++) {
        for(uint32_t col = 0; col < gm->cols; col++) {
            if (gm->values[row][col] == 0) {
                c++;
            }
        }
        if(c == gm->cols) {
            r++;
        }
        c = 0;
    }
    uint32_t ans = m->cols - r;
    mod_matrix_free(gm);
    return ans;
}

matrix_t * inverse(matrix_t * m) {
    matrix_t * extended = merge(m, ones(m->rows, m->cols));
    matrix_t * gauss = algorithms_gauss_row_reduction(extended);

    matrix_t * inverse = mod_matrix_new(m->rows, gauss->cols / 2);
    uint32_t * * values_i = inverse->values;
    uint32_t * * values_g = gauss->values;
    //TODO CHECK THIS
    for(uint32_t i = 0; i < inverse->rows; i++) {
        for(uint32_t j = 0; j < inverse->cols; j ++){
             values_i[i][j] = values_g[i][j+m->cols];
        }
    }
    mod_matrix_free(gauss);
    mod_matrix_free(extended);
    return inverse;
}

matrix_t * projection(matrix_t * m) {
    matrix_t * transposed = mod_matrix_transpose(m);

    matrix_t * a = mod_matrix_mul(transposed,m);
    print_matrix(a);
    matrix_t * b = inverse(a);
    matrix_t * c = mod_matrix_mul(m, b);
    matrix_t * d = mod_matrix_mul(c, transposed);
    return d;
}


