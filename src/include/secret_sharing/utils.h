#ifndef UTILS_H_
#define UTILS_H_

#include <bmp/bmp.h>

int check_shadow_sizes(BMPImage * secret,  BMPImage ** shadows, size_t len, int n, int k);

#endif // UTILS_H_
