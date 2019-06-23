#include "bmp.h"
#include "recover.h"
#include "stegano.h"
#include "matrix_operations.h"
#include "utils.h"
#include <stdlib.h>

static inline void get_sub_matrix_into_image(matrix * sm, BMPImage * s, uint32_t base);
static inline void get_sub_matrix_from_matrix(matrix *s, matrix *secret, uint32_t base);

void recover(char * filename, BMPImage **shadows, BMPImage *rw,uint32_t k, uint32_t n) {
    double width_factor = (1.0 / k + 1.0 / n);
    BMPImage *secret = build_image(shadows[0]), *watermark = build_image(shadows[0]);
    uint32_t secret_rows = secret->header.height_px, secret_cols = (uint32_t) (secret->header.width_px * width_factor);

    matrix *B = matrix_create_new(n, k);
    matrix *R = matrix_create_new(n, n);
    matrix *Rw = matrix_create_new(n, n);
    matrix *Gg = matrix_create_new(k, 1);
    matrix *I = matrix_create_new(k, k);
    for (uint32_t row = 0; row < I->rows; row++) {
        for (uint32_t col = 0; col < I->cols; col++) {
            I->values[row][col] = (int_pow(shadows[row]->header.reserved1 + 1, col)) % MOD;
        }
    }
    I = inverse(I);

    matrix **G = malloc(sizeof(*G) * k);
    for (uint32_t i = 0; i < k; i++) {
        G[i] = matrix_create_new(n, 2);
    }

    matrix **Sh = malloc(sizeof(*Sh) * k);
    for (uint32_t i = 0; i < k; i++) {
        Sh[i] = matrix_create_new(n, 3);
    }

    matrix **shares = malloc(sizeof(*shares) * k);
    for (uint32_t i = 0; i < k; i++) {
        shares[i] = matrix_create_new(secret_rows, secret_cols);
    }
    // Recover shares from stegano
    for (uint32_t j = 0; j < k; j++) {
        for (uint32_t i = 0; i < shadows[j]->header.image_size_bytes; i += n) {
            int col = (i/n) % shares[j]->cols;
            int row = (i/n) / shares[j]->cols;
            shares[j]->values[row][col] = recover_lsb(shadows[j]->data + i, n);
        }
    }

    for (uint32_t share_image_k = 0, secret_k = 0;share_image_k < (shares[0]->rows * shares[0]->cols);) {

        for (uint32_t i = 0; i < k; i++) {
            get_sub_matrix_from_matrix(Sh[i], shares[i], share_image_k);
        }

        for (uint32_t i = 0; i < k; i++) {
            for (uint32_t i1 = 0; i1 < n; i1++) {
                B->values[i1][i] = Sh[i]->values[i1][0];
            }
        }

        matrix *Sd = matrix_projection(B);

        for (uint32_t i = 0; i < k; i++) {
            for (uint32_t i1 = 0; i1 < n; i1++) {
                for (uint32_t j1 = 0; j1 < G[i]->cols; j1++) {
                    G[i]->values[i1][j1] = Sh[i]->values[i1][j1+1];
                }
            }
        }

        for (uint32_t i1 = 0; i1 < R->rows; ++i1) {
            for (uint32_t j1 = 0; j1 < (R->cols / k); j1++) {

                for (uint32_t i = 0; i < k; i++) {
                    Gg->values[i][0] = G[i]->values[i1][j1];
                }
                matrix * Ii = matrix_multiply(I, Gg);

                for (uint32_t i = 0; i < k; i++) {
                    R->values[i1][j1 * k + i] = Ii->values[i][0];
                }
                matrix_free(Ii);
            }
        }

        matrix *S = matrix_sum(Sd, R);
        get_sub_matrix_into_image(S, secret, secret_k);

        get_sub_matrix_from_image(Rw, rw, secret_k);
        matrix *W = matrix_sum(Sd, Rw);
        get_sub_matrix_into_image(W, watermark, secret_k);

        share_image_k += (n * 3);
        secret_k += (n * n);

        matrix_free(Sd);
        matrix_free(S);
        matrix_free(W);
    }

    FILE *fd = fopen(filename, "wb");
    write_bmp(secret, fd);

    FILE *watermark_file = fopen("wm.bmp", "wb");
    write_bmp(watermark, watermark_file);
    free_bmp(watermark);
    fclose(watermark_file);

    //mem down
    matrix_free(B);
    matrix_free(R);
    matrix_free(Rw);
    matrix_free(Gg);
    matrix_free(I);

    for(uint32_t i = 0; i < k; i++) {
        matrix_free(G[i]);
        matrix_free(Sh[i]);
        matrix_free(shares[i]);
    }

    free(G);
    free(Sh);
    free(shares);
}

static inline void get_sub_matrix_into_image(matrix * sm, BMPImage * s, uint32_t base){
    for (uint32_t k = 0; k < sm->rows * sm->cols; k++) {
        uint32_t j = (k) % sm->cols;
        uint32_t i = (k) / sm->cols;
        s->data[base + k] = sm->values[i][j];
    }
}

static inline void get_sub_matrix_from_matrix(matrix *s, matrix *secret, uint32_t base) {
    for (uint32_t k = 0; k < s->rows * s->cols; k++) {
        uint32_t j = (base + k) % secret->cols;
        uint32_t i = (base + k) / secret->cols;
        s->values[k / s->cols][k % s->cols] = (uint32_t) secret->values[i][j];
    }
}