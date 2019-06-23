#include "include/utils.h"
#include <steg/steg.h>
#include <dirent.h>
#include <bmp/bmp.h>
#include <string.h>

bool contains(uint8_t *array, int size, uint8_t a);

void rand_set_seed(int64_t s){
    rand_seed = (s ^ 0x5DEECE66DL) & ((1LL << 48) - 1);
}

uint8_t rand_next_char(void) {
    rand_seed = (rand_seed * 0x5DEECE66DL + 0xBL) & ((1LL << 48) - 1);
    return (uint8_t)(rand_seed>>40);
}

void hide_shares(matrix **shares, BMPImage **shadows, int n) {

    FILE * fd;
    BMPImage * shadow;
    uint32_t k, i, j;
    char file_name[11] = "shareN.bmp";
    for (k = 0; k < n; k++) {
        shadow = shadows[k];
        for (i = 0; i < shares[k]->rows; i++) {
            for(j = 0; j < shares[k]-> cols; j++) {
                // Steganography
                distribute_lsb(shares[k]->values[i][j], shadow->data + j * n, n);
            }
        }

        // This is for the I matrix in the recover algorithm
        shadow->header.reserved1 = k + 1;
        // Change the N for the number of the share
        file_name[5] = k +'0' + 1;
        fd = fopen(file_name, "wb");
        write_bmp(shadow, fd);

    }

}

void merge_sub_matrix_into_matrix(matrix * m, matrix *sub_matrix, int base_row, int base_col) {
    for(int row=0; row < sub_matrix->rows; row++){
        for (int col = 0; col < sub_matrix->cols; col++) {
            m->values[base_row+row][base_col+col] = sub_matrix->values[row][col];
        }
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
                    uint32_t value = R->values[i][j + l];
                    int pow = int_pow((t + 1), l);
                    sum += (value * pow);

                }
                G[t]->values[i][z] = sum % MOD;
                z++;
            }
            z = 0;
        }
    }
}

bool contains(uint8_t *array, int size, uint8_t a) {
    for (int i = 0; i < size; i++) {
        if(array[i] == a){
            return 1;
        }
    }
    return 0;
}

void create_random_x_vectors(matrix **X, int n, int k) {
    uint8_t * a_array = malloc(sizeof(*a_array)*n);
    uint8_t a, a_pow;
    for (int j = 0; j < n; j++) {
    do {
        a = modulo(rand_next_char());
    } while (contains(a_array, j, a));
    a_array[j] = a;
    for (int row = 0; row < k; row++) {
        a_pow = (uint8_t) (int_pow(a, row) % MOD);
        X[j]->values[row][0] = a_pow;
    }
  }
}

void create_random_A_matrix(matrix *A) {
  do {
    for (int row = 0; row < A->rows; row++) {
      for (int col = 0; col < A->cols; col++) {
        A->values[row][col] = (uint32_t)(modulo(rand_next_char()));
      }
    }
  } while (rank(A) != A->cols || rank(matrix_multiply(matrix_transpose(A),A)) != A->cols);

}

void get_sub_matrix_from_image(matrix *s, BMPImage *secret, int base_col, int base_row) {
    for(int row=0; row < s->rows; row++ ){
        for (int col = 0; col < s->cols; col++) {
            s->values[row][col] = (uint32_t) secret->data[((base_row+row)*secret->header.width_px) + base_col + col];
            if(s->values[row][col] > 250) {
              s->values[row][col] = 250;
            }
        }
    }
}

void get_sub_matrix_from_matrix(matrix *s, matrix *secret, int base_col, int base_row) {
    for(int row=0; row < s->rows; row++ ){
        for (int col = 0; col < s->cols; col++) {
            s->values[row][col] = (uint32_t) secret->values[base_row+row][base_col + col];
        }
    }
}

void get_sub_matrix_into_image(matrix *s, BMPImage *secret, int base_col, int base_row) {
    for(int row=0; row < s->rows; row++ ){
        for (int col = 0; col < s->cols; col++) {
            secret->data[((base_row+row)*secret->header.width_px) + base_col + col] = s->values[row][col];
        }
    }
}

void
save_rw_to_image(BMPImage * s, matrix * rw) {
  BMPImage * rw_image = copy_bmp(s);

  /** Save rw matrix into image */
  for(uint32_t row = 0; row < rw->rows; row++) {
    for(uint32_t col = 0; col < rw->cols; col++) {
      rw_image->data[row*rw->cols + col] = rw->values[row][col];
    }
  }

  /** Write bmp to rw file */
  FILE * rw_image_file = fopen("rw.bmp", "wb");
  write_bmp(rw_image, rw_image_file);

  /** Free used resources */
  free_bmp(rw_image);
  fclose(rw_image_file);
}

uint32_t int_pow(uint32_t x, uint32_t y){
    // X^Y
    if( y == 0){
        return 1;
    }
    uint32_t ret = x;
    for (int i = 0; i < y - 1; i++) {
        ret *= x;
    }
    return ret;
}

BMPImage * build_image(BMPImage * base) {
  BMPImage * new_image = copy_bmp(base);
  new_image->extra_header = calloc(256 * 4, sizeof(uint8_t)); //Reservar espacio para la paleta de colores
  new_image->header.bits_per_pixel = 8; // 8 bits por pixel
  new_image->header.size = base->header.width_px * base->header.height_px + 1024;
  // El tamaño completo de la imagen es width * heigth + el espacio de la paleta
  new_image->header.offset = 1078; //Es 54 + 1024
  new_image->header.num_colors = 256; // 256 colores
  new_image->header.image_size_bytes = base->header.width_px * base->header.height_px;

  /* Generacion de la paleta de colores */
  int jj=3;
  for(int ii=0;ii<255;ii++){
    new_image->extra_header[jj+1]=(uint8_t)ii+1;
    new_image->extra_header[jj+2]=(uint8_t)ii+1;
    new_image->extra_header[jj+3]=(uint8_t)ii+1;
    jj=jj+4;
  }

  return new_image;
}

size_t shadow_size_for(uint32_t image_size, int n, int k) {
    int lsb_bytes;
    if(k == 4) {
        lsb_bytes = 8;
    } else {
        lsb_bytes = 4;
    }
    double factor = (1.0/k + 1.0/n);

    return image_size * factor * lsb_bytes;
}

int check_shadow_sizes(BMPImage * secret,  BMPImage ** shadows, size_t len, int n, int k) {
    BMPHeader secret_header = secret->header;

    uint32_t real_byte_count = secret_header.width_px  * secret_header.height_px;

    size_t shadow_size = shadow_size_for(real_byte_count, n, k);
    int i;

    for (i = 0; i < len; i++){
        BMPHeader header = shadows[i]->header;
        size_t shadow_real_byte_count = header.image_size_bytes;

        if (shadow_real_byte_count != shadow_size)
        {
            return -1;
        }

    }
    return 0;
}
