//
// Created by Agustin Vazquez on 2019-06-15.
//

#ifndef GATSS_MATRIX_OPERATIONS_H
#define GATSS_MATRIX_OPERATIONS_H
#include "matrix_arithmetic.h"

matrix * matrix_inverse(matrix * m);

matrix * matrix_projection(matrix * m);

uint32_t rank(matrix * m);

#endif //GATSS_MATRIX_OPERATIONS_H
