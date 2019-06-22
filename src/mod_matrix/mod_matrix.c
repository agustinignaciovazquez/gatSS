#include <mod_matrix/mod_matrix.h>
#include <algorithms/algorithms.h>
#include <mod_matrix/mod_matrix_macros.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <logger/logger.h>

static inline matrix_t*
mod_matrix_basic_op_apply(matrix_t* a,matrix_t* b,uint32_t (*f)(uint32_t, uint32_t)){
  if (a->cols != b->cols || a->rows != b->rows) {
    printf("Matrix sizes does not match!");
    exit(EXIT_FAILURE);
  }

  matrix_t* r = mod_matrix_new(a->rows, a->cols);

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

matrix_t*
mod_matrix_sum(matrix_t* a, matrix_t* b){
  return mod_matrix_basic_op_apply(a, b, mod_sum);
}

matrix_t*
mod_matrix_sub(matrix_t* a, matrix_t* b)
{
  return mod_matrix_basic_op_apply(a, b, mod_sub);
}

matrix_t*
mod_matrix_mul(matrix_t* a, matrix_t* b)
{
    if (a->cols != b->rows) {
        printf("Incorrect multiply matrix dimensions!");
        exit(EXIT_FAILURE);
    }

    matrix_t* r = mod_matrix_new(a->rows, b->cols);

    uint32_t dim = a->cols;
    uint32_t sum = 0;

    for (uint32_t i = 0; i < r->rows; i++) {
        for (uint32_t j = 0; j < r->cols; j++) {
            for (uint32_t k = 0; k < dim; k++) {
                sum = mod_sum(sum,mod_mul(a->values[i][k],b->values[k][j]));
            }
            r->values[i][j] = sum;
            sum = 0;
        }
    }

    return r;
}

matrix_t* mod_matrix_transpose(matrix_t* m){
  matrix_t* r = mod_matrix_new(m->cols, m->rows);
  uint32_t * * values_r = r->values;
  uint32_t * * values_m = m->values;

  for (uint32_t i = 0; i < r->rows; i++) {
    for (uint32_t j = 0; j < r->cols; j++) {
        values_r[i][j] = values_m[j][i];
    }
  }

  return r;
}

matrix_t * ones(uint32_t rows, uint32_t cols) {
    matrix_t * r = mod_matrix_new(rows, cols);

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

matrix_t * matrix_create_copy(matrix_t * m){
    matrix_t * r = mod_matrix_new(m->rows,m->cols);
    matrix_copy(m,r);
    return r;
}

void matrix_copy(matrix_t * from, matrix_t * to){
    if (from->rows != to->rows || from->cols != to->cols) {
        printf("Incorrect matrix dimensions!");
        exit(EXIT_FAILURE);
    }

    uint32_t * * values_from = from->values;
    uint32_t * * values_to = to->values;

    //TODO Use memcpy in columns for better performance
    for(uint32_t i = 0; i < to->rows; i++) {
        for(uint32_t j = 0; j < to->cols; j ++){
            values_to[i][j] = values_from[i][j];
        }
    }
}

matrix_t * merge(matrix_t * a, matrix_t * b) {
    if (a->rows != b->rows) {
        printf("Incorrect matrix dimensions!");
        exit(EXIT_FAILURE);
    }

    matrix_t * r = mod_matrix_new(a->rows, a->cols + b->cols);
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

void print_matrix(matrix_t * matrix) {
    for(uint32_t i = 0; i < matrix->rows; i++) {
        for(uint32_t j = 0; j < matrix->cols; j++) {
            printf("%d ", matrix->values[i][j]);
        }
        printf("\n");
    }
}

static inline void mod_free_dynamic_matrix(uint32_t * * matrix, uint32_t rows){
    uint32_t i;
    for(i=0; i < rows;i++){
        free(matrix[i]);
    }
    free(matrix);
}

static inline char * * create_dynamic_matrix(uint32_t rows, uint32_t cols){
    char * * matrix;
    matrix = malloc(rows*sizeof(uint32_t *));
    if(matrix != NULL){
        for(uint32_t i = 0;i < rows;i++){
            matrix[i] = malloc(cols*sizeof(uint32_t));
            if(matrix[i] == NULL){
                mod_free_dynamic_matrix(matrix,i);
                matrix = NULL;
                break;
            }
        }
    }
    return matrix;
}

matrix_t*
mod_matrix_new(uint32_t rows, uint32_t cols) {
  matrix_t * m = malloc(sizeof(matrix_t));
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

void mod_matrix_free(matrix_t* m){
    mod_free_dynamic_matrix(m->values,m->rows);
    free(m);
}


