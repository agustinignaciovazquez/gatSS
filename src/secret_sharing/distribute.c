#include <mod_matrix/mod_matrix.h>
#include <algorithms/algorithms.h>
#include <mod_matrix/mod_matrix_macros.h>
#include <secret_sharing/distribute.h>
#include "utils.h"
#include <stdlib.h>

void distribute(int k, int n, BMPImage * secret, BMPImage ** shadows, BMPImage * watermark) {

    int secret_row, secret_col, share_row = 0, share_col = 0, i, total_secret_rows, total_secret_cols;

    // Create matrix for the algorithm

    matrix_t ** shares;
    shares = malloc(sizeof(*shares)*n);
    total_secret_rows = secret->header.height_px;
    double width_factor = (1.0/k + 1.0/n);
    total_secret_cols = (int)(secret->header.width_px*width_factor);

    for (i = 0; i < n; ++i) {
        shares[i] = mod_matrix_new(total_secret_rows, total_secret_cols);
        shares[i]->rows = total_secret_rows;
        shares[i]->cols = total_secret_cols;
    }

    // Create matrix for watermark remainder
    matrix_t * rw = mod_matrix_new(secret->header.height_px, secret->header.width_px);

    matrix_t * S = mod_matrix_new(n,n);
    S->cols = n;
    S->rows = n;

    matrix_t * W = mod_matrix_new(n, n);

    matrix_t * A = mod_matrix_new(n,k);
    A->cols = k;
    A->rows = n;

    matrix_t ** X = malloc(n * sizeof(matrix_t *));

    for(i = 0; i < n; i++) {
        X[i] = mod_matrix_new(k,1);
        X[i]->rows = k;
        X[i]->cols = 1;
    }

    matrix_t ** V = malloc(n * sizeof(matrix_t *));

    matrix_t ** Sh = malloc(n * sizeof(matrix_t *));

    matrix_t ** G = malloc(n * sizeof(matrix_t *));

    for(i = 0; i < n; i++) {
        G[i] = mod_matrix_new(n,2);
        G[i]->rows = n;
        G[i]->cols = 2;
    }


    for (secret_row = 0, share_row = 0; secret_row < secret->header.height_px; secret_row += n, share_row += n) {
        for (secret_col = 0, share_col = 0; secret_col < secret->header.width_px; secret_col += n, share_col += 3) {

            create_random_A_matrix(A);
            print_matrix(A);
            matrix_t * Sd = projection(A);

            get_sub_matrix_from_image(S, secret, secret_col, secret_row);

            matrix_t * R = mod_matrix_sub(S, Sd);

            create_random_x_vectors(X, n, k);

            // Generate V vectors
            for(int z = 0; z < n; z ++) {
                V[z] = mod_matrix_mul(A, X[z]);
            }

            // Generate Rw matrix
            get_sub_matrix_from_image(W, watermark, secret_col, secret_row);
            matrix_t * current_rw = mod_matrix_sub(W, Sd);
            merge_sub_matrix_into_matrix(rw, current_rw, secret_row, secret_col);
            free(current_rw);

            generate_G_matrix(G, R, n ,k);

            for(i = 0; i < n; i ++) {
                Sh[i] = merge(V[i], G[i]);
            }

            for (int l = 0; l < n; l++) {
                merge_sub_matrix_into_matrix(shares[l], Sh[l], share_row, share_col);
            }

        }
    }

    hide_shares(shares, shadows, n);
    save_rw_to_image(secret, rw);

    free(A);
    free(S);
    free(Sh);
    free(X);
    free(V);
    free(G);
    free(W);
    free(rw);

}
