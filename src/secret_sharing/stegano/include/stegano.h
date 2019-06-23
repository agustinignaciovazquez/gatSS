#ifndef STEG_H
#define STEG_H

#include <stdint.h>
#include <unistd.h>

void distribute_lsb(uint8_t byte, uint8_t *pixels, int n);

void lsb_2(uint8_t byte, uint8_t *pixels, size_t pos);

void lsb_1(uint8_t byte, uint8_t *pixels, size_t pos);

uint8_t recover_lsb(uint8_t *bytes, int n);

uint8_t rec_lsb_2(uint8_t *bytes, size_t pos);

uint8_t rec_lsb_1(uint8_t *bytes, size_t pos);

#endif // STEG_H
