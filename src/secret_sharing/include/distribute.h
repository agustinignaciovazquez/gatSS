//
// Created by Agustin Vazquez on 2019-06-19.
//

#ifndef GATSS_DISTRIBUTE_H
#define GATSS_DISTRIBUTE_H

#include "bmp.h"

void distribute(uint32_t k, uint32_t n, BMPImage * secret, BMPImage ** shadows, BMPImage * watermark);
void recover(uint32_t k, uint32_t n, char *secret_file_name, BMPImage ** shadows, BMPImage * rw);

#endif //GATSS_DISTRIBUTE_H
