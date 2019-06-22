#ifndef STEG_H
#define STEG_H

#include <stdint.h>
#include <unistd.h>

uint8_t recover_lsb(uint8_t *bytes, int n);

void distribute_lsb(uint8_t byte, uint8_t *pixels, int n);

void distribute_lsb_width2(uint8_t byte, uint8_t *pixels, size_t pos);

void distribute_lsb_width1(uint8_t byte, uint8_t *pixels, size_t pos);

uint8_t recover_lsb_width2(uint8_t *bytes, size_t pos);

uint8_t recover_lsb_width1(uint8_t *bytes, size_t pos);


#endif // STEG_H
