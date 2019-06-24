#ifndef GATSS_STEGANO_H
#define GATSS_STEGANO_H

#include <stdint.h>
#include <unistd.h>

void distribute_lsb(uint8_t byte, uint8_t *pixels, uint32_t n);

void lsb_2(uint8_t byte, uint8_t *pixels, size_t pos);

void lsb_1(uint8_t byte, uint8_t *pixels, size_t pos);

uint8_t recover_lsb(uint8_t *bytes, uint32_t n);

uint8_t rec_lsb_2(uint8_t *bytes, size_t pos);

uint8_t rec_lsb_1(uint8_t *bytes, size_t pos);


#endif
