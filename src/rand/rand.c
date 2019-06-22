#include <rand/rand.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

void
set_seed(int64_t s)
{
    seed = (s ^ 0x5DEECE66DL) & ((1LL << 48) - 1);
}
uint8_t
next_char(void) {
    seed = (seed * 0x5DEECE66DL + 0xBL) & ((1LL << 48) - 1);
    return (uint8_t)(seed>>40);
}

uint8_t
safe_next_char(void) {
    uint8_t  res = next_char();
    while (res >= 251){
        res = next_char();
    }
    return res;
}
