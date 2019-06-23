//
// Created by Agustin Vazquez on 2019-06-19.
//

#ifndef GATSS_RECOVER_H
#define GATSS_RECOVER_H

#include "bmp.h"

void recover(uint32_t k, uint32_t n, char *secret_file_name, BMPImage ** shadows, BMPImage * rw);

#endif //GATSS_RECOVER_H
