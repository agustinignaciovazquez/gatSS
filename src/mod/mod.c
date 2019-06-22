#include <mod/mod.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t
mod_sum(uint32_t a, uint32_t b)
{
    return modulo(a + b);
}

uint32_t
mod_sub2(uint32_t a, uint32_t b)
{
    long double x;
    uint32_t c;
    int32_t r;
    if (a >= MOD)
        a %= MOD;
    if (b >= MOD)
        b %= MOD;
    x = a;
    c = x * b / MOD;
    r = (int32_t)(a * b - c * MOD) % (int32_t)MOD;
    return r < 0 ? r + MOD : r;
}

uint32_t
mod_sub(uint32_t a, uint32_t b)
{
    uint32_t ans = modulo(a) - modulo(b);
    if (a < b){
        return(ans + MOD);
    }

    return ans;
}

uint32_t
mod_mul(uint32_t a, uint32_t b)
{
    uint32_t c;
    int32_t r;

    uint32_t mod_a = modulo(a);
    uint32_t mod_b = modulo(b);

    c = 1.0*mod_a * mod_b / MOD;
    r = (int32_t)(mod_a * mod_b - c * MOD) % (int32_t)MOD;
    return r < 0 ? r + MOD : r;
}

uint32_t
mod_div(uint32_t a, uint32_t b)
{
    return mod_mul(a, inv_multip(b));
}

