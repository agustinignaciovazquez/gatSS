//
// Created by tsoracc on 2019-06-15.
//

#include "stegano.h"
#include <stdlib.h>
#include <stdio.h>

#define BIT(n) (0x01 << (n))
#define GET_BIT(x, n) (((x) >> (n)) & 1)

void distribute_lsb(uint8_t byte, uint8_t *pixels, uint32_t n){
    if(n == 4) {
        lsb_2(byte, pixels, 0);
    } else if(n == 8) {
        lsb_1(byte, pixels, 0);
    }
}

void lsb_2(uint8_t byte, uint8_t *pixels, size_t pos){
    uint32_t j;
    for (j = 0; j < 8; j += 2){
        uint32_t i = j / 2;
        uint8_t hide = (GET_BIT(byte, 8 - 1 - j) << 1) | GET_BIT(byte, 8 - 1 - (j + 1));
        pixels[i] = (pixels[i] & ~(BIT(pos + 1) | BIT(pos))) | (hide << pos);
    }
}

void lsb_1(uint8_t byte, uint8_t *pixels, size_t pos){
    uint32_t i;
    for (i = 0; i < 8; i++) {
        pixels[i] = (pixels[i] & ~BIT(pos)) | (GET_BIT(byte, 8 - 1 - i) << pos);
    }
}

uint8_t recover_lsb(uint8_t *bytes, uint32_t n){
    if(n == 4) {
        return rec_lsb_2(bytes, 0);
    } else if(n == 8) {
        return rec_lsb_1(bytes, 0);
    }

    printf("Arg error: supported schemes are (2,4) and (4,8)\n");
    exit(EXIT_FAILURE);
}

uint8_t rec_lsb_2(uint8_t *bytes, size_t pos){
    uint32_t i;
    uint8_t byte = 0;
    uint8_t recover;
    for (i = 0; i < 4; i++){
        recover = bytes[i] & (0x03 << pos);
        recover = recover >> pos;
        byte |= (recover << (8 - 2 - (i * 2)));
    }

    return byte;
}

uint8_t rec_lsb_1(uint8_t *bytes, size_t pos){
    uint32_t i;
    uint8_t byte = 0;
    uint8_t recover;
    for (i = 0; i < 8; i++) {
        recover = bytes[i] & BIT(pos);
        recover = recover >> pos;
        byte |= (recover << (8 - 1 - i));
    }

    return byte;
}