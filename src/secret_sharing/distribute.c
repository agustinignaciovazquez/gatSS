#include "matrix_operations.h"
#include "distribute.h"
#include "utils.h"
#include "stegano.h"
#include <stdlib.h>
static inline void rand_A_verified_into_matrix(matrix *A);
static inline void fill_G_into_matrix(matrix **G, matrix *R, uint32_t n, uint32_t k);
static inline void rw_into_bmp(BMPImage *s, matrix *rw);
static inline void fill_rand_X_into(matrix **X, uint32_t n, uint32_t k);
static inline void stegano_into_image(matrix **shares, BMPImage **shadows, uint32_t n);

void distribute(uint32_t k, uint32_t n, BMPImage *secret, BMPImage **shadows, BMPImage *watermark) {

    int secret_position, share_position = 0, i, total_secret_rows, total_secret_cols;

    // Create matrix for the algorithm

    matrix **shares;
    shares = malloc(sizeof(*shares) * n);
    total_secret_rows = secret->header.height_px;
    double width_factor = (1.0 / k + 1.0 / n);
    total_secret_cols = (int) (secret->header.width_px * width_factor);

    for (i = 0; i < n; ++i) {
        shares[i] = matrix_create_new(total_secret_rows, total_secret_cols);
    }

    // Create matrix for watermark remainder
    matrix *rw = matrix_create_new(secret->header.height_px, secret->header.width_px);

    matrix *S = matrix_create_new(n, n);

    matrix *W = matrix_create_new(n, n);

    matrix *A = matrix_create_new(n, k);

    matrix **X = malloc(n * sizeof(matrix *));

    for (i = 0; i < n; i++) {
        X[i] = matrix_create_new(k, 1);
    }

    matrix **V = malloc(n * sizeof(matrix *));

    matrix **Sh = malloc(n * sizeof(matrix *));

    matrix **G = malloc(n * sizeof(matrix *));

    for (i = 0; i < n; i++) {
        G[i] = matrix_create_new(n, 2);
    }

    // Start the algorithm
    for (secret_position = 0, share_position = 0;
         secret_position < secret->header.image_size_bytes; secret_position += n * n, share_position += n * 3) {


        rand_A_verified_into_matrix(A);

        matrix *Sd = matrix_projection(A);

        get_sub_matrix_from_image(S, secret, secret_position);

        matrix *R = matrix_subtract(S, Sd);

        fill_rand_X_into(X, n, k);

        // Generate V vectors
        for (int z = 0; z < n; z++) {
            V[z] = matrix_multiply(A, X[z]);
        }

        // Generate Rw matrix
        get_sub_matrix_from_image(W, watermark, secret_position);
        matrix *current_rw = matrix_subtract(W, Sd);
        get_sub_matrix_into_matrix(rw, current_rw, secret_position);
        free(current_rw);

        fill_G_into_matrix(G, R, n, k);

        for (i = 0; i < n; i++) {
            Sh[i] = matrix_merge(V[i], G[i]);
        }

        for (int l = 0; l < n; l++) {
            get_sub_matrix_into_matrix(shares[l], Sh[l], share_position);
        }

    }

    stegano_into_image(shares, shadows, n);
    rw_into_bmp(secret, rw);

}

static inline void stegano_into_image(matrix **shares, BMPImage **shadows, uint32_t n) {

    FILE *fd;
    BMPImage *shadow;
    uint32_t k, j;
    char file_name[11] = "shareN.bmp";
    for (k = 0; k < n; ++k) {
        shadow = shadows[k];

        for(uint32_t i = 0; i < shares[0]->rows; i++) {
            for (j = 0; j < shares[k]->cols; j++) {
                // Steganography
                distribute_lsb(shares[k]->values[i][j], shadow->data + j * n, n);
            }
        }

        // This is for the I matrix in the recover algorithm
        shadow->header.reserved1 = k;
        // Change the N for the number of the share
        file_name[5] = k + '0' + 1;
        fd = fopen(file_name, "wb");
        write_bmp(shadow, fd);

    }

}


static inline void fill_G_into_matrix(matrix **G, matrix *R, uint32_t n, uint32_t k) {
    uint32_t i1 = 0;
    for (uint32_t i2 = 0; i2 < n; i2++) {
        uint32_t s = 0;
        for (uint32_t i = 0; i < R->rows; i++) {
            for (uint32_t j = 0; j < R->cols; j += k) {
                s = 0;
                for (uint32_t l = 0; l < k; ++l) {
                    uint32_t value = R->values[i][j + l];
                    uint32_t pow = int_pow((i2 + 1), l);
                    s += (value * pow);

                }
                G[i2]->values[i][i1] = s % MOD;
                i1++;
            }
            i1 = 0;
        }
    }
}

static inline bool array_contains(uint8_t * arr, uint32_t n, uint8_t a) {
    for (uint32_t i = 0; i < n; i++) {
        if(arr[i] == a){
            return 1;
        }
    }
    return 0;
}

static inline void fill_rand_X_into(matrix ** X, uint32_t n, uint32_t k) {
    uint8_t * a_array = malloc(sizeof(*a_array)*n);
    uint8_t a, a_pow;
    for (uint32_t j = 0; j < n; j++) {
        do {
            a = rand_next_char();
        } while (array_contains(a_array, j, a) == 1);

        a_array[j] = a;
        for (uint32_t row = 0; row < k; row++) {
            a_pow = (uint8_t) (int_pow(a, row) % MOD);
            X[j]->values[row][0] = a_pow;
        }
    }
}

static inline void rand_A_verified_into_matrix(matrix * A) {
    do {
        for (uint32_t i = 0; i < A->rows; i++) {
            for (uint32_t j = 0; j < A->cols; j++) {
                A->values[i][j] = (uint32_t)(rand_next_char());
            }
        }
    } while (rank(A) != A->cols || rank(matrix_multiply(matrix_transpose(A),A)) != A->cols);

}

static inline void rw_into_bmp(BMPImage *s, matrix *rw) {
    BMPImage *rw_image = copy_bmp(s);

    for (uint32_t row = 0; row < rw->rows; row++) {
        for (uint32_t col = 0; col < rw->cols; col++) {
            rw_image->data[row * rw->cols + col] = rw->values[row][col];
        }
    }

    FILE *rw_image_file = fopen("rw.bmp", "wb");
    write_bmp(rw_image, rw_image_file);

    free_bmp(rw_image);
    fclose(rw_image_file);
}
