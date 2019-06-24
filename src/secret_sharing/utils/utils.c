#include "utils.h"
#include "stegano.h"
#include "bmp.h"
#include <dirent.h>
#include <string.h>

void rand_set_seed(int64_t s){
    rand_seed = (s ^ 0x5DEECE66DL) & ((1LL << 48) - 1);
}

static inline uint8_t next_char(void) {
    rand_seed = (rand_seed * 0x5DEECE66DL + 0xBL) & ((1LL << 48) - 1);
    return (uint8_t)(rand_seed>>40);
}

uint8_t rand_next_char(void) {
    uint8_t  r;
    do {
        r = next_char();
    }while(r >= MOD);
    return r;
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

void sm_merge_matrix(matrix *m, matrix *sm, uint32_t base) {
    for (uint32_t k = 0; k < sm->rows * sm->cols; k++) {
        uint32_t j = (base + k) % m->cols;
        uint32_t i = (base + k) / m->cols;
        m->values[i][j] = sm->values[(k) / sm->cols][k % sm->cols];
    }
}

void sm_from_bmp(matrix * sm, BMPImage * s, uint32_t base) {
    uint32_t t;
    for (uint32_t k = 0; k < sm->rows * sm->cols; k++) {
        uint32_t j = (k) % sm->cols;
        uint32_t i = (k) / sm->cols;
        t = (uint32_t) s->data[base + k];
        sm->values[i][j] = (t > (MOD -1))?(MOD - 1):t;
    }
}
