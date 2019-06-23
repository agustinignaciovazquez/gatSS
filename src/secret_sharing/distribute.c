#include "mod_256/include/matrix_operations.h"
#include <secret_sharing/distribute.h>
#include "include/utils.h"
#include <stdlib.h>

void distribute(int k, int n, BMPImage * secret, BMPImage ** shadows, BMPImage * watermark) {

    int secret_row, secret_col, share_row = 0, share_col = 0, i, total_secret_rows, total_secret_cols;

    // Create matrix for the algorithm

    matrix ** shares;
    shares = malloc(sizeof(*shares)*n);
    total_secret_rows = secret->header.height_px;
    double width_factor = (1.0/k + 1.0/n);
    total_secret_cols = (int)(secret->header.width_px*width_factor);

    for (i = 0; i < n; ++i) {
        shares[i] = matrix_create_new(total_secret_rows, total_secret_cols);
        shares[i]->rows = total_secret_rows;
        shares[i]->cols = total_secret_cols;
    }

    // Create matrix for watermark remainder
    matrix * rw = matrix_create_new(secret->header.height_px, secret->header.width_px);

    matrix * S = matrix_create_new(n,n);

    matrix * W = matrix_create_new(n, n);

    matrix * A = matrix_create_new(n,k);

    matrix ** X = malloc(n * sizeof(matrix *));

    for(i = 0; i < n; i++) {
        X[i] = matrix_create_new(k,1);
    }

    matrix ** V = malloc(n * sizeof(matrix *));

    matrix ** Sh = malloc(n * sizeof(matrix *));

    matrix ** G = malloc(n * sizeof(matrix *));

    for(i = 0; i < n; i++) {
        G[i] = matrix_create_new(n,2);
    }


    for (secret_row = 0, share_row = 0; secret_row < secret->header.height_px; secret_row += n, share_row += n) {
        for (secret_col = 0, share_col = 0; secret_col < secret->header.width_px; secret_col += n, share_col += 3) {

            create_random_A_matrix(A);

            matrix * Sd = projection(A);

            get_sub_matrix_from_image(S, secret, secret_col, secret_row);

            matrix * R = matrix_subtract(S, Sd);

            create_random_x_vectors(X, n, k);

            // Generate V vectors
            for(int z = 0; z < n; z ++) {
                V[z] = matrix_multiply(A, X[z]);
            }

            // Generate Rw matrix
            get_sub_matrix_from_image(W, watermark, secret_col, secret_row);
            matrix * current_rw = matrix_subtract(W, Sd);
            merge_sub_matrix_into_matrix(rw, current_rw, secret_row, secret_col);
            matrix_free(current_rw);

            generate_G_matrix(G, R, n ,k);

            for(i = 0; i < n; i ++) {
                Sh[i] = matrix_merge(V[i], G[i]);
            }

            for (int l = 0; l < n; l++) {
                merge_sub_matrix_into_matrix(shares[l], Sh[l], share_row, share_col);
            }

        }
    }

    hide_shares(shares, shadows, n);
    save_rw_to_image(secret, rw);

    matrix_free(S);
    matrix_free(A);
    matrix_free(W);
    matrix_free(X);
    matrix_free(V);
    matrix_free(rw);
    matrix_free(Sh);
    matrix_free(G);
}
