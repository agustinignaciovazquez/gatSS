//
// Created by Agustin Vazquez on 2019-06-19.
//

#ifndef GATSS_DISTRIBUTE_H
#define GATSS_DISTRIBUTE_H

#include "bmp.h"

void distribute(BMPImage *secret, BMPImage ** shadows, BMPImage * wm,uint32_t k, uint32_t n);

#endif //GATSS_DISTRIBUTE_H
