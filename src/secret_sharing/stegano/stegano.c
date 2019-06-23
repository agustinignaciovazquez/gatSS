#include "stegano.h"
#include <stdlib.h>
#include <stdio.h>

#define BIT(n) (0x01 << (n))
#define GET_BIT(x, n) (((x) >> (n)) & 1)

void distribute_lsb(uint8_t byte, uint8_t *pixels, int n){
    if(n == 4) {
        distribute_lsb_width2(byte, pixels, 0);
    } else if(n == 8) {
        distribute_lsb_width1(byte, pixels, 0);
    }
}


uint8_t recover_lsb(uint8_t *bytes, int n){
    if(n == 4) {
        return recover_lsb_width2(bytes, 0);
    } else if(n == 8) {
        return recover_lsb_width1(bytes, 0);
    }

    printf("Error incorrect N (we only accept 4 or 8)\n");
    exit(EXIT_FAILURE);
}

void distribute_lsb_width2(uint8_t byte, uint8_t *pixels, size_t pos)
{
    int j;
    for (j = 0; j < 8; j += 2)
    {
        int i = j / 2;
        uint8_t to_save = (GET_BIT(byte, 8 - 1 - j) << 1) | GET_BIT(byte, 8 - 1 - (j + 1));
        pixels[i] = (pixels[i] & ~(BIT(pos + 1) | BIT(pos))) | (to_save << pos);
    }
}

void distribute_lsb_width1(uint8_t byte, uint8_t *pixels, size_t pos) {
    int i;
    for (i = 0; i < 8; i++) {
        pixels[i] = (pixels[i] & ~BIT(pos)) | (GET_BIT(byte, 8 - 1 - i) << pos);
    }
}

uint8_t recover_lsb_width2(uint8_t *bytes, size_t pos)
{
    int i;
    uint8_t byte = 0;
    for (i = 0; i < 4; i++)
    {
        uint8_t bits = bytes[i] & (0x03 << pos);
        bits = bits >> pos;
        byte |= (bits << (8 - 2 - (i * 2)));
    }

    return byte;
}

uint8_t recover_lsb_width1(uint8_t *bytes, size_t pos) {
    int i;
    uint8_t byte = 0;
    for (i = 0; i < 8; i++) {
        uint8_t bit = bytes[i] & BIT(pos);
        bit = bit >> pos;
        byte |= (bit << (8 - 1 - i));
    }

    return byte;
}
