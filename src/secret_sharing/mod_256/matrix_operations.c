//
// Created by Agustin Vazquez on 2019-06-15.
//

#include "matrix_operations.h"
#include <stdio.h>
#include <string.h>

static inline uint8_t find_non_zero_col(const matrix *matrix, uint32_t start, uint32_t col, uint32_t *r);
static inline void swap_rows(matrix *m, uint32_t row1, uint32_t row2);
matrix * matrix_gauss_algorithm(matrix* m1);

matrix* matrix_gauss_algorithm(matrix* m1){
    uint32_t lead_coef_row = 0,lead_coef_col = 0, lead_coef_value = 0, temp;
    matrix* m = matrix_create_copy(m1);

    while (lead_coef_row < m->rows && lead_coef_col < m->cols) {
        temp = 0;

        if (find_non_zero_col(m, lead_coef_row, lead_coef_col, &temp) == 0) {
            lead_coef_value = 0;
        } else {
            //Found first non-zero element
            lead_coef_value = m->values[temp][lead_coef_col];
        }

        if (lead_coef_value != 0) {
            swap_rows(m, lead_coef_row, temp);

            for (uint32_t col = lead_coef_col; col < m->cols; col++) {
                m->values[lead_coef_row][col] = divide(m->values[lead_coef_row][col], lead_coef_value);
            }

            uint32_t row = ((lead_coef_row == 0) ? 1 : 0);
            do{
                uint32_t t = m->values[row][lead_coef_col];
                for (uint32_t col = lead_coef_col; col < m->cols; col++) {
                    m->values[row][col] = subtract(m->values[row][col], multiply(t, m->values[lead_coef_row][col]));
                }
                row += ((row == (lead_coef_row - 1)) ? 2 : 1);
            }while ( row < m->rows);

            lead_coef_row++;
            lead_coef_col++;
        } else {
            for (uint32_t row = lead_coef_row; row < m->rows; row++) {
                m->values[row][lead_coef_col] = 0;
            }
            lead_coef_col++;
        }
    }

    return m;
}

uint32_t rank(matrix * m) {
    matrix * gm = matrix_gauss_algorithm(matrix_transpose(m));
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
    matrix_free(gm);
    return ans;
}

matrix * matrix_inverse(matrix * m) {
    matrix * extended = matrix_merge(m, matrix_identity(m->rows, m->cols));
    matrix * gauss = matrix_gauss_algorithm(extended);

    matrix * inverse = matrix_create_new(m->rows, gauss->cols / 2);
    uint32_t * * values_i = inverse->values;
    uint32_t * * values_g = gauss->values;

    //Desuamentanto de lado la matriz identidad
    for(uint32_t i = 0; i < inverse->rows; i++) {
        for(uint32_t j = 0; j < inverse->cols; j ++){
            values_i[i][j] = values_g[i][j+m->cols];
        }
    }

    //Liberando memoria
    matrix_free(gauss);
    matrix_free(extended);
    return inverse;
}

matrix * matrix_projection(matrix * m) {
    matrix * transposed = matrix_transpose(m);
    matrix * a = matrix_multiply(transposed,m);
    matrix * b = matrix_inverse(a);
    matrix * c = matrix_multiply(m, b);
    matrix * d = matrix_multiply(c, transposed);

    //Liberando memoria
    matrix_free(transposed);
    matrix_free(a);
    matrix_free(b);
    matrix_free(c);
    return d;
}

static inline uint8_t find_non_zero_col(const matrix *matrix, uint32_t start, uint32_t col, uint32_t *r){
    for (uint32_t row = start; row < matrix->rows; row++) {
        if (matrix->values[row][col] != 0) {
            *r = row;
            return 1;
        }
    }
    return 0;
}

static inline void swap_rows(matrix *m, uint32_t row1, uint32_t row2){
    uint32_t * temp;
    temp = m->values[row1];
    m->values[row1] = m->values[row2];
    m->values[row2] = temp;
}
