//
// Created by Julian Palacci on 2019-06-08.
//

#ifndef MODLIB_RAND_H
#define MODLIB_RAND_H

#include <stdint.h>

/*variable global*/
int64_t seed; /*seed debe ser de 48 bits; se elige este tipo de 64 bits*/
void set_seed(int64_t seed);

uint8_t
next_char(void); /*devuelve un unsigned char*/

uint8_t
safe_next_char(void);

#endif //MODLIB_RAND_H
