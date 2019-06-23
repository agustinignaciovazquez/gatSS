//
// Created by Agustin Vazquez on 2019-06-10.
//

#include "integer_arithmetic.h"

uint32_t sum(uint32_t a, uint32_t b){
    return modulo(a + b);
}

uint32_t subtract(uint32_t a, uint32_t b){
    uint32_t ans = modulo(a) - modulo(b);
    if (a < b){
        return(ans + MOD);
    }

    return ans;
}

uint32_t multiply(uint32_t a, uint32_t b){
    uint32_t c;
    int32_t r;

    uint32_t mod_a = modulo(a);
    uint32_t mod_b = modulo(b);

    c = 1.0*mod_a * mod_b / MOD;
    r = (int32_t)(mod_a * mod_b - c * MOD) % (int32_t)MOD;
    return r < 0 ? r + MOD : r;
}

uint32_t divide(uint32_t a, uint32_t b){
    return multiply(a, inv_multip(b));
}
