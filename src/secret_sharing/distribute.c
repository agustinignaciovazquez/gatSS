#include "matrix_operations.h"
#include "distribute.h"
//#include "utils.h"
#include <stdlib.h>

void distribute(int k, int n, BMPImage *secret, BMPImage **shadows, BMPImage *watermark) {

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


        create_random_A_matrix(A);

        matrix *Sd = matrix_projection(A);

        get_sub_matrix_from_image(S, secret, secret_position);

        matrix *R = matrix_subtract(S, Sd);

        create_random_x_vectors(X, n, k);

        // Generate V vectors
        for (int z = 0; z < n; z++) {
            V[z] = matrix_multiply(A, X[z]);
        }

        // Generate Rw matrix
        get_sub_matrix_from_image(W, watermark, secret_position);
        matrix *current_rw = matrix_subtract(W, Sd);
        get_sub_matrix_into_matrix(rw, current_rw, secret_position);
        free(current_rw);

        generate_G_matrix(G, R, n, k);

        for (i = 0; i < n; i++) {
            Sh[i] = matrix_merge(V[i], G[i]);
        }

        for (int l = 0; l < n; l++) {
            get_sub_matrix_into_matrix(shares[l], Sh[l], share_position);
        }

    }

    hide_shares(shares, shadows, n);
    save_rw_to_image(secret, rw);

}
