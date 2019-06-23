#include "utils.h"
#include "stegano.h"
#include "bmp.h"
#include <dirent.h>
#include <string.h>

bool contains(uint8_t *array, int size, uint8_t a);

void rand_set_seed(int64_t s){
    rand_seed = (s ^ 0x5DEECE66DL) & ((1LL << 48) - 1);
}

uint8_t rand_next_char(void) {
    rand_seed = (rand_seed * 0x5DEECE66DL + 0xBL) & ((1LL << 48) - 1);
    return (uint8_t)(rand_seed>>40);
}

uint8_t safe_next_char(void) {
    uint8_t  r;
    do {
        r = rand_next_char();
    }while(r >= 251);
    return r;
}

void hide_shares(matrix **shares, BMPImage **shadows, int n) {

    FILE *fd;
    BMPImage *shadow;
    int k, j;
    char file_name[11] = "shareN.bmp";
    for (k = 0; k < n; ++k) {
        shadow = shadows[k];

        for(int i = 0; i < shares[0]->rows; i++) {
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

void generate_G_matrix(matrix **G, matrix *R, int n, int k) {
    int z = 0;
    for (int t = 0; t < n; t++) {
        int sum = 0;
        for (int i = 0; i < R->rows; i++) {
            for (int j = 0; j < R->cols; j += k) {
                sum = 0;
                for (int l = 0; l < k; ++l) {
                    uint32_t value = R->values[i][j+l];
                    int pow = int_pow((t + 1), l);
                    sum += (value * pow);

                }
                G[t]->values[i][z] = sum % 251;
                z++;
            }
            z = 0;
        }
    }
}

bool contains(uint8_t *array, int size, uint8_t a) {
    for (int i = 0; i < size; i++) {
        if (array[i] == a) {
            return 1;
        }
    }
    return 0;
}

void create_random_x_vectors(matrix **X, int n, int k) {
    uint8_t *a_array = malloc(sizeof(*a_array) * n);
    uint8_t a, a_pow;
    for (int j = 0; j < n; j++) {
        do {
            a = safe_next_char();
        } while (contains(a_array, j, a));
        a_array[j] = a;
        for (int row = 0; row < k; row++) {
            a_pow = (uint8_t) (int_pow(a, row) % 251);
            X[j]->values[row][0] = a_pow;
        }
    }
}


void create_random_A_matrix(matrix *A) {
    do {
        for (int row = 0; row < A->rows; row++) {
            for (int col = 0; col < A->cols; col++) {
                A->values[row][col] = (uint32_t) safe_next_char();
            }
        }
    } while (rank(A) != A->cols || rank(matrix_multiply(matrix_transpose(A), A)) != A->cols);

}

void get_sub_matrix_from_image(matrix *s, BMPImage *secret, int base_position) {
    for (int position = 0; position < s->rows * s->cols; position++) {
        int col = (position) % s->cols;
        int row = (position) / s->cols;
        s->values[row][col] = (uint32_t) secret->data[base_position + position];
        if (s->values[row][col] > 250) {
            s->values[row][col] = 250;
        }
    }

}

void get_sub_matrix_from_matrix(matrix *s, matrix *secret, int base_position) {
    for (int position = 0; position < s->rows * s->cols; position++) {
        int col = (base_position + position) % secret->cols;
        int row = (base_position + position) / secret->cols;
        s->values[position / s->cols][position % s->cols] = (uint32_t) secret->values[row][col];
    }
}

void get_sub_matrix_into_image(matrix *s, BMPImage *secret, int base_position){
    for (int position = 0; position < s->rows * s->cols; position++) {
        int col = (position) % s->cols;
        int row = (position) / s->cols;
        secret->data[base_position + position] = s->values[row][col];
    }
}

void get_sub_matrix_into_matrix(matrix *m, matrix *sub_matrix, int base_position) {
    for (int position = 0; position < sub_matrix->rows * sub_matrix->cols; position++) {
        int col = (base_position + position) % m->cols;
        int row = (base_position + position) / m->cols;
        m->values[row][col] = sub_matrix->values[(position) / sub_matrix->cols][position % sub_matrix->cols];
    }
}

void
save_rw_to_image(BMPImage *s, matrix *rw) {
    BMPImage *rw_image = copy_bmp(s);

    /** Save rw matrix into image */
    for (uint32_t row = 0; row < rw->rows; row++) {
        for (uint32_t col = 0; col < rw->cols; col++) {
            rw_image->data[row * rw->cols + col] = rw->values[row][col];
        }
    }

    /** Write bmp to rw file */
    FILE *rw_image_file = fopen("rw.bmp", "wb");
    write_bmp(rw_image, rw_image_file);

    /** Free used resources */
    free_bmp(rw_image);
    fclose(rw_image_file);
}

uint32_t int_pow(uint32_t x, uint32_t y) {
    // X^Y
    if (y == 0) {
        return 1;
    }
    uint32_t ret = x;
    for (int i = 0; i < y - 1; i++) {
        ret *= x;
    }
    return ret;
}


size_t shadow_size_for(uint32_t image_size, int n, int k) {
    int lsb_bytes;
    if (k == 4) {
        lsb_bytes = 8;
    } else {
        lsb_bytes = 4;
    }
    double factor = (1.0 / k + 1.0 / n);

    return image_size * factor * lsb_bytes;
}

int check_shadow_sizes(BMPImage *secret, BMPImage **shadows, size_t len, int n, int k) {
    BMPHeader secret_header = secret->header;

    uint32_t real_byte_count = secret_header.width_px * secret_header.height_px;

    size_t shadow_size = shadow_size_for(real_byte_count, n, k);
    int i;

    for (i = 0; i < len; i++) {
        BMPHeader header = shadows[i]->header;
        size_t shadow_real_byte_count = header.image_size_bytes;

        if (shadow_real_byte_count != shadow_size) {
            return -1;
        }

    }

    return 0;
}
