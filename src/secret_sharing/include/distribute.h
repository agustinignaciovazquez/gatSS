//
// Created by Agustin Vazquez on 2019-06-19.
//

#ifndef GATSS_DISTRIBUTE_H
#define GATSS_DISTRIBUTE_H

#include "bmp.h"

void distribute(int k, int n, BMPImage * secret, BMPImage ** shadows, BMPImage * watermark);
void recover(int k, int n, char *secret_file_name, BMPImage ** shadows, BMPImage * rw);

#endif //GATSS_DISTRIBUTE_H
