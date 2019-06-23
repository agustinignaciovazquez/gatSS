#include "utils.h"
#include "stegano.h"
#include "bmp.h"
#include <dirent.h>
#include <string.h>

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

void get_sub_matrix_into_matrix(matrix *m, matrix *sm, uint32_t base) {
    for (uint32_t k = 0; k < sm->rows * sm->cols; k++) {
        uint32_t j = (base + k) % m->cols;
        uint32_t i = (base + k) / m->cols;
        m->values[i][j] = sm->values[(k) / sm->cols][k % sm->cols];
    }
}

void get_sub_matrix_from_image(matrix * sm, BMPImage * s, uint32_t base) {
    uint32_t t;
    for (uint32_t k = 0; k < sm->rows * sm->cols; k++) {
        uint32_t j = (k) % sm->cols;
        uint32_t i = (k) / sm->cols;
        t = (uint32_t) s->data[base + k];
        sm->values[i][j] = (t > 250)? 250 : t;
    }
}

void get_sub_matrix_into_image(matrix * sm, BMPImage * s, uint32_t base){
    for (uint32_t k = 0; k < sm->rows * sm->cols; k++) {
        uint32_t j = (k) % sm->cols;
        uint32_t i = (k) / sm->cols;
        s->data[base + k] = sm->values[i][j];
    }
}

void get_sub_matrix_from_matrix(matrix *s, matrix *secret, uint32_t base) {
    for (uint32_t k = 0; k < s->rows * s->cols; k++) {
        uint32_t j = (base + k) % secret->cols;
        uint32_t i = (base + k) / secret->cols;
        s->values[k / s->cols][k % s->cols] = (uint32_t) secret->values[i][j];
    }
}

uint32_t int_pow(uint32_t x, uint32_t y) {
    if (y == 0) {
        return 1;
    }
    uint32_t ret = x;
    for (uint32_t i = 0; i < y - 1; i++) {
        ret *= x;
    }
    return ret;
}


size_t shadow_size_for(uint32_t image_size, uint32_t n, uint32_t k) {
    uint32_t lsb_bytes;
    if (k == 4) {
        lsb_bytes = 8;
    } else {
        lsb_bytes = 4;
    }
    double factor = (1.0 / k + 1.0 / n);

    return image_size * factor * lsb_bytes;
}

uint32_t check_shadow_sizes(BMPImage *secret, BMPImage **shadows, size_t len, uint32_t n, uint32_t k) {
    BMPHeader secret_header = secret->header;
    uint32_t real_byte_count = secret_header.width_px * secret_header.height_px;
    size_t shadow_size = shadow_size_for(real_byte_count, n, k);

    for (uint32_t i = 0; i < len; i++) {
        BMPHeader header = shadows[i]->header;
        size_t byte_count = header.image_size_bytes;
        if (byte_count != shadow_size) {
            return -1;
        }
    }
    return 0;
}
