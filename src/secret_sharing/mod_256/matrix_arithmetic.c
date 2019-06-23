//
// Created by Agustin Vazquez on 2019-06-10.
//

#include "include/matrix_arithmetic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void matrix_dynamic_free(uint32_t * * matrix, uint32_t rows);
static inline uint32_t * * create_dynamic_matrix(uint32_t rows, uint32_t cols);
static inline matrix * matrix_apply_op(matrix* a,matrix* b,uint32_t (*f)(uint32_t, uint32_t));

matrix * matrix_sum(matrix* a, matrix* b){
    return matrix_apply_op(a, b, sum);
}

matrix * matrix_subtract(matrix* a, matrix* b)
{
    return matrix_apply_op(a, b, subtract);
}

matrix * matrix_identity(uint32_t rows, uint32_t cols) {
    matrix * r = matrix_create_new(rows, cols);

    for(uint32_t i = 0; i < r->rows; i++) {
        for(uint32_t j = 0; j < r->cols; j++) {
            if(i == j)
                r->values[i][j] = 1;
            else
                r->values[i][j] = 0;
        }
    }
    return r;
}

matrix* matrix_transpose(matrix* m){
    matrix* r = matrix_create_new(m->cols, m->rows);
    uint32_t * * values_r = r->values;
    uint32_t * * values_m = m->values;

    for (uint32_t i = 0; i < r->rows; i++) {
        for (uint32_t j = 0; j < r->cols; j++) {
            values_r[i][j] = values_m[j][i];
        }
    }

    return r;
}

matrix* matrix_multiply(matrix* a, matrix* b){
    if (a->cols != b->rows) {
        printf("Incorrect multiply matrix dimensions!");
        exit(EXIT_FAILURE);
    }

    matrix* r = matrix_create_new(a->rows, b->cols);

    uint32_t dim = a->cols;
    uint32_t s = 0;

    for (uint32_t i = 0; i < r->rows; i++) {
        for (uint32_t j = 0; j < r->cols; j++) {
            for (uint32_t k = 0; k < dim; k++) {
                s = sum(s,multiply(a->values[i][k],b->values[k][j]));
            }
            r->values[i][j] = s;
            s = 0;
        }
    }

    return r;
}

matrix * matrix_create_copy(matrix * m){
    matrix * r = matrix_create_new(m->rows,m->cols);
    matrix_copy(m,r);
    return r;
}

void matrix_copy(matrix * from, matrix * to){
    if (from->rows != to->rows || from->cols != to->cols) {
        printf("Incorrect matrix dimensions!");
        exit(EXIT_FAILURE);
    }

    uint32_t * * values_from = from->values;
    uint32_t * * values_to = to->values;

    //Best perfomance ??
    for(uint32_t i = 0; i < to->rows; i++) {
        for(uint32_t j = 0; j < to->cols; j ++){
            values_to[i][j] = values_from[i][j];
        }
    }
}

matrix * matrix_merge(matrix * a, matrix * b) {
    if (a->rows != b->rows) {
        printf("Incorrect matrix dimensions!");
        exit(EXIT_FAILURE);
    }

    matrix * r = matrix_create_new(a->rows, a->cols + b->cols);
    uint32_t * * values_r = r->values;
    uint32_t * * values_a = a->values;
    uint32_t * * values_b = b->values;
    for(uint32_t i = 0; i < a->rows; i++) {
        for(uint32_t j = 0; j < a->cols; j ++){
            values_r[i][j] = values_a[i][j];
        }
        for(uint32_t j = 0; j < b->cols; j ++){
            values_r[i][a->cols + j] = values_b[i][j];
        }
    }

    return r;
}

matrix * matrix_create_new(uint32_t rows, uint32_t cols){
    matrix * m = malloc(sizeof(matrix));
    if(m == NULL)
        return m;

    m->rows = rows;
    m->cols = cols;
    m->values = create_dynamic_matrix(rows,cols);

    if(m->values == NULL){
        free(m);
        return NULL;
    }

    return m;
}

void matrix_free(matrix* m){
    matrix_dynamic_free(m->values,m->rows);
    free(m);
}

void matrix_print(matrix * matrix) {
    for(uint32_t i = 0; i < matrix->rows; i++) {
        for(uint32_t j = 0; j < matrix->cols; j++) {
            printf("%d ", matrix->values[i][j]);
        }
        printf("\n");
    }
}

static inline void matrix_dynamic_free(uint32_t * * matrix, uint32_t rows){
    uint32_t i;
    for(i=0; i < rows;i++){
        free(matrix[i]);
    }
    free(matrix);
}

static inline uint32_t * * create_dynamic_matrix(uint32_t rows, uint32_t cols){
    uint32_t * * matrix_value;
    matrix_value = malloc(rows*sizeof(uint32_t *));
    if(matrix_value != NULL){
        for(uint32_t i = 0;i < rows;i++){
            matrix_value[i] = malloc(cols*sizeof(uint32_t));
            if(matrix_value[i] == NULL){
                matrix_dynamic_free(matrix_value,i);
                return NULL;
            }
        }
    }
    return matrix_value;
}

static inline matrix * matrix_apply_op(matrix* a,matrix* b,uint32_t (*f)(uint32_t, uint32_t)){
    if (a->cols != b->cols || a->rows != b->rows) {
        printf("Incorrect matrix dimension!");
        exit(EXIT_FAILURE);
    }

    matrix* r = matrix_create_new(a->rows, a->cols);

    uint32_t * * values_r = r->values;
    uint32_t * *values_a = a->values;
    uint32_t * *values_b = b->values;

    for(uint32_t i = 0; i < r->rows;i++){
        for(uint32_t j = 0;j < r->cols;j++){
            values_r[i][j] = f(values_a[i][j],values_b[i][j]);
        }
    }

    return r;
}
