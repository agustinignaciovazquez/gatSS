#include "bmp.h"
#include "recover.h"
#include "stegano.h"
#include "matrix_operations.h"
#include "utils.h"
#include <stdlib.h>


void recover(int k, int n, char *secret_file_name, BMPImage ** shadows, BMPImage * rw) {

    int i = 0, j = 0, row = 0, col = 0, total_secret_rows, total_secret_cols;

    BMPImage * secret = build_image(shadows[0]);
    BMPImage * watermark = build_image(shadows[0]);

    total_secret_rows = secret->header.height_px;
    double width_factor = (1.0/k + 1.0/n);
    total_secret_cols = (int)(secret->header.width_px*width_factor);

    matrix ** shares = malloc(sizeof(*shares)* k);
    for (i = 0; i < k; i++) {
        shares[i] = matrix_create_new(total_secret_rows, total_secret_cols);
    }

    matrix ** Sh = malloc(sizeof(*Sh) * k);
    for (i = 0; i < k; i++) {
        Sh[i] = matrix_create_new(n,3);
    }

    matrix ** G = malloc(sizeof(*G) * k);
    for (i = 0; i < k; i++) {
        G[i] = matrix_create_new(n,2);
    }

    matrix * B = matrix_create_new(n, k);

    matrix* R = matrix_create_new(n, n);

    matrix * I = matrix_create_new(k, k);

    matrix * Rw = matrix_create_new(n, n);

    for (row = 0; row < I->rows; row++) {
        for(col = 0; col < I->cols; col++) {
            I->values[row][col] = (int_pow(shadows[row]->header.reserved1, col)) % MOD;
        }
    }
    I = inverse(I);
    matrix * Ii;
    matrix * S;
    matrix * W;

    matrix * Gg = matrix_create_new(k,1);

    for (j = 0; j < k; j++) {
        for (i = 0; i < shadows[j]->header.image_size_bytes; i += n) {
            int col = (i/n) % shares[j]->cols;
            int row = (i/n) / shares[j]->cols;
            shares[j]->values[row][col] = recover_lsb(shadows[j]->data + i, n);
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
                    B->values[row][i] = Sh[i]->values[row][0];
                }
            }

            matrix * Sd = projection(B);

            for (i = 0; i < k; i++) {
                for (row = 0; row < n; row++) {
                    for (col = 0; col < G[i]->cols ; col++) {
                        G[i]->values[row][col] = Sh[i]->values[row][col+1];
                    }
                }
            }

            for (row = 0; row < R->rows; ++row) {
                for (col = 0; col < (R->cols/k); col++) {
                    for(i = 0; i < k; i++) {
                        Gg->values[i][0] = G[i]->values[row][col];
                    }

                    Ii = matrix_multiply(I, Gg);

                    for(i = 0; i < k; i++) {
                        R->values[row][col*k + i] = Ii->values[i][0];
                    }

                }
            }

          S = matrix_sum(Sd, R);
          get_sub_matrix_into_image(S, secret, secret_col, secret_row);

          /** Create Watermark */
          get_sub_matrix_from_image(Rw, rw, secret_col, secret_row);
          W = matrix_sum(Sd, Rw);
          get_sub_matrix_into_image(W, watermark, secret_col, secret_row);
          matrix_free(W);

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
