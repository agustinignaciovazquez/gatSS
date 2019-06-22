#include <bmp/bmp.h>
#include <mod_matrix/mod_matrix.h>
#include <algorithms/algorithms.h>
#include <mod_matrix/mod_matrix_macros.h>
#include <secret_sharing/recover.h>
#include <steg/steg.h>
#include "utils.h"

void recover(int k, int n, char *secret_file_name, BMPImage ** shadows, BMPImage * rw) {

    int i = 0, j = 0, row = 0, col = 0, total_secret_rows, total_secret_cols;

    BMPImage * secret = build_image(shadows[0]);
    BMPImage * watermark = build_image(shadows[0]);

    total_secret_rows = secret->header.height_px;
    double width_factor = (1.0/k + 1.0/n);
    total_secret_cols = (int)(secret->header.width_px*width_factor);

    matrix_t ** shares = malloc(sizeof(*shares)* k);
    for (i = 0; i < k; i++) {
        shares[i] = NEW_MATRIX(total_secret_rows, total_secret_cols);
        shares[i]->cols = total_secret_cols;
        shares[i]->rows = total_secret_rows;
    }

    matrix_t ** Sh = malloc(sizeof(*Sh) * k);
    for (i = 0; i < k; i++) {
        Sh[i] = NEW_MATRIX(n,3);
        Sh[i]->cols = 3;
        Sh[i]->rows = n;
    }

    matrix_t ** G = malloc(sizeof(*G) * k);
    for (i = 0; i < k; i++) {
        G[i] = NEW_MATRIX(n,2);
        G[i]->cols = 2;
        G[i]->rows = n;
    }

    matrix_t * B = NEW_MATRIX(n, k);
    B->rows = n;
    B->cols = k;

    matrix_t* R = NEW_MATRIX(n, n);
    R->cols = n;
    R->rows = n;

    matrix_t * I = NEW_MATRIX(k, k);
    I->cols = k;
    I->rows = k;

    matrix_t * Rw = mod_matrix_new(n, n);

    for (row = 0; row < I->rows; row++) {
        for(col = 0; col < I->cols; col++) {
            I->values[row * I->cols + col] = (int_pow(shadows[row]->header.reserved1, col)) % 251;
        }
    }
    I = inverse(I);

    matrix_t * Ii;
    matrix_t * S;
    matrix_t * W;

    matrix_t * Gg = NEW_MATRIX(k,1);
    Gg->rows = k;
    Gg->cols = 1;

    for (j = 0; j < k; j++) {
        for (i = 0; i < shadows[j]->header.image_size_bytes; i += n) {
            shares[j]->values[i/n] = recover_lsb(shadows[j]->data + i, n);
        }
    }


    // The shares are of equal size
    for (int share_image_row = 0, secret_row = 0; share_image_row < shares[0]->rows; share_image_row += n, secret_row += n) {

        for (int share_image_col = 0, secret_col = 0; share_image_col < shares[0]->cols; share_image_col += 3, secret_col += n) {


            for (i = 0; i < k; i++) {
                get_sub_matrix_from_matrix(Sh[i], shares[i], share_image_col, share_image_row);
            }

            for (i = 0; i < k; i++) {
                for (row = 0; row < n; row++) {
                    ELEM(B, row, i) = ELEM(Sh[i], row, 0);
                }
            }

            matrix_t * Sd = projection(B);

            for (i = 0; i < k; i++) {
                for (row = 0; row < n; row++) {
                    for (col = 0; col < G[i]->cols ; col++) {
                        ELEM(G[i], row, col) = ELEM(Sh[i],row ,col+1);
                    }
                }
            }

            for (row = 0; row < R->rows; ++row) {
                for (col = 0; col < (R->cols/k); col++) {
                    for(i = 0; i < k; i++) {
                        ELEM(Gg, i, 0) = ELEM(G[i],row, col);
                    }

                    Ii = mod_matrix_mul(I, Gg);

                    for(i = 0; i < k; i++) {
                        ELEM(R,row, col*k + i) = ELEM(Ii, i, 0);
                    }

                }
            }

          S = mod_matrix_sum(Sd, R);
          get_sub_matrix_into_image(S, secret, secret_col, secret_row);

          /** Create Watermark */
          get_sub_matrix_from_image(Rw, rw, secret_col, secret_row);
          W = mod_matrix_sum(Sd, Rw);
          get_sub_matrix_into_image(W, watermark, secret_col, secret_row);
          free(W);

        }
    }

    FILE * fd = fopen(secret_file_name, "wb");
    write_bmp(secret, fd);

    /** Save watermark to file */
    FILE * watermark_file = fopen("watermark.bmp", "wb");
    write_bmp(watermark, watermark_file);
    free_bmp(watermark);
    fclose(watermark_file);
}
