#include "matrix_operations.h"
#include "distribute.h"
#include "utils.h"
#include "stegano.h"
#include <stdlib.h>

#define ASCII_DIG(i) (i + '0')

static inline void rand_A_verified_into_matrix(matrix *A);
static inline void fill_G_into_matrix(matrix **G, matrix *R, uint32_t n, uint32_t k);
static inline void rw_into_bmp(BMPImage *s, matrix *rw);
static inline void fill_rand_X_into(matrix **X, uint32_t n, uint32_t k);
static inline void stegano_into_image(matrix **shares, BMPImage **shadows, uint32_t n);

void distribute(BMPImage *secret, BMPImage ** shadows, BMPImage * wm,uint32_t k, uint32_t n) {
    double width_factor = (1.0 / k + 1.0 / n);
    uint32_t secret_rows = secret->header.height_px, secret_cols = (uint32_t) (secret->header.width_px * width_factor);

    matrix *rw = matrix_create_new(secret->header.height_px, secret->header.width_px);
    matrix *A = matrix_create_new(n, k);
    matrix *W = matrix_create_new(n, n);
    matrix *S = matrix_create_new(n, n);

    matrix **V = malloc(n * sizeof(matrix *));
    matrix **Sh = malloc(n * sizeof(matrix *));

    matrix **X = malloc(n * sizeof(matrix *));
    for (uint32_t i = 0; i < n; i++) {
        X[i] = matrix_create_new(k, 1);
    }

    matrix **G = malloc(n * sizeof(matrix *));
    for (uint32_t i = 0; i < n; i++) {
        G[i] = matrix_create_new(n, 2);
    }

    matrix **shares = malloc(sizeof(*shares) * n);
    for (uint32_t i = 0; i < n; ++i) {
        shares[i] = matrix_create_new(secret_rows, secret_cols);
    }

    for (uint32_t secret_k = 0, share_k = 0;secret_k < secret->header.image_size_bytes; secret_k += n * n, share_k += n * 3) {
        rand_A_verified_into_matrix(A);
        //matrix_print(A);
        matrix *Sd = matrix_projection(A);
        //matrix_print(Sd);
        sm_from_bmp(S, secret, secret_k);
        //matrix_print(S);
        matrix *R = matrix_subtract(S, Sd);
        //matrix_print(R);
        fill_rand_X_into(X, n, k);

        // Generate V vectors
        for (uint32_t i = 0; i < n; i++) {
            V[i] = matrix_multiply(A, X[i]);
        }

        // Generate Rw matrix
        sm_from_bmp(W, wm, secret_k);
        matrix *current_rw = matrix_subtract(W, Sd);
        sm_merge_matrix(rw, current_rw, secret_k);
        //matrix_print(rw);

        fill_G_into_matrix(G, R, n, k);

        for (uint32_t i = 0; i < n; i++) {
            Sh[i] = matrix_merge(V[i], G[i]);
        }

        for (uint32_t i = 0; i < n; i++) {
            sm_merge_matrix(shares[i], Sh[i], share_k);
        }

        matrix_free(R);
        matrix_free(Sd);
        matrix_free(current_rw);
        for(uint32_t i = 0; i < n; i++) {
            matrix_free(Sh[i]);
            matrix_free(V[i]);
        }
    }
    //Save shares hidden into images
    stegano_into_image(shares, shadows, n);
    rw_into_bmp(secret, rw);

    matrix_free(S);
    matrix_free(A);
    matrix_free(W);
    matrix_free(rw);

    for(uint32_t i = 0; i < n; i++) {
        matrix_free(G[i]);
        matrix_free(X[i]);
        matrix_free(shares[i]);
    }

    free(G);
    free(X);
    free(Sh);
    free(V);
    free(shares);
}

static inline void stegano_into_image(matrix **shares, BMPImage **shadows, uint32_t n) {
    char filename[20] = "shares_N.bmp";
    for (uint32_t k = 0; k < n; ++k) {
        BMPImage *shadow = shadows[k];
        for (uint32_t p = 0; p < shares[0]->rows * shares[k]->cols; p++) {
            distribute_lsb(shares[k]->values[p/shares[k]->cols][p%shares[k]->cols], shadow->data + p * n, n);
        }
        filename[7] = ASCII_DIG(k+1);
        shadow->header.reserved1 = k;
        FILE *fd = fopen(filename, "wb");
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
