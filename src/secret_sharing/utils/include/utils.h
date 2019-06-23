#ifndef UTILS_H
#define UTILS_H

#include "bmp.h"
#include "matrix_operations.h"
#include <stdlib.h>

int64_t rand_seed;

uint8_t rand_next_char(void);

void rand_set_seed(int64_t s);

/**
 * @brief get a sub matrix s from an image starting at base_col and base_row.
 *
 * @param matrix* s
 * @param BMPImage* secret
 * @param int base_col
 * @param int base_row
 * @return void
 */
void get_sub_matrix_from_image(matrix *s, BMPImage *secret, int base_position);

/**
 * @brief get a sub matrix s into an image starting at base_col and base_row.
 *
 * @param matrix* s
 * @param BMPImage* secret
 * @param int base_col
 * @param int base_row
 * @return void
 */
void get_sub_matrix_into_image(matrix *s, BMPImage *secret, int base_position);

/**
 * @brief get a sub matrix into another matrix starting at base_col and base_row.
 *
 * @param matrix* matrix
 * @param matrix* sub_matrix
 * @param int base_col
 * @param int base_row
 * @return void
 */
void get_sub_matrix_into_matrix(matrix *m, matrix *sub_matrix, int base_position);

/**
 * @brief get a sub matrix from another matrix starting at base_col and base_row.
 *
 * @param matrix* s
 * @param matrix* secret
 * @param int base_col
 * @param int base_row
 * @return void
 */
void get_sub_matrix_from_matrix(matrix *s, matrix *secret, int base_position);

/**
 * @brief fill the matrix A with random values using safe_next_char. This matrix is of full rank and
 * the matrix product of A*A^t is also of full rank
 *
 * @param matrix* A
 * @return void
 */
void create_random_A_matrix(matrix *A);

/**
 * @brief fill the vectors in X  with random values using safe_next_char. This vectors are linear independant between
 * each others. This vectors are created using a Vandermont matrix
 *
 * @param matrix** X
 * @param int n, number of vectors in X
 * @param int k, rows in the vectors
 * @return void
 */
void create_random_x_vectors(matrix **X, int n, int k);

/**
 * @brief fill the vectors in G using the formula in Shamir's Secret Sharing Scheme
 *
 * @param matrix** G
 * @param matrix* R
 * @param int n
 * @param int k
 * @return void
 */
void generate_G_matrix(matrix **G, matrix *R, int n, int k);

/**
 * @brief use steganography to hide the shares into the images. The images output name will be shareN.bmp
 * where the N represent the number of the share
 *
 * @param matrix** shares
 * @param BMPImage** shadows
 * @param int n
 * @return void
 */
void hide_shares(matrix **shares, BMPImage **shadows, int n);

/**
 * @brief X^Y
 *
 * @param uint_32 x
 * @param uint_32 y
 * @return uint32_t
 */
uint32_t int_pow(uint32_t x, uint32_t y);

/**
 * @brief Save the Rw matrix into an image
 *
 * @param uint_32 x
 * @param uint_32 y
 * @return void
 */
void save_rw_to_image(BMPImage * s, matrix * rw);

size_t shadow_size_for(uint32_t image_size, int n, int k);

int check_shadow_sizes(BMPImage * secret,  BMPImage ** shadows, size_t len, int n, int k);

#endif // UTILS_H
