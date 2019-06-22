#include <mod/mod.h>
#include <stdio.h>
#include <stdlib.h>

static inline uint32_t
mod_reduce(uint32_t x)
{
    return (x >= MOD) ? (x % MOD) : x;
}

uint32_t
mod_sum(uint32_t a, uint32_t b)
{
    return mod_reduce(a + b);
}

uint32_t
mod_sub(uint32_t a, uint32_t b)
{
    return (mod_reduce(a) + (a < b ? MOD : 0)) - mod_reduce(b);
}

uint32_t
mod_mul(uint32_t a, uint32_t b)
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
mod_div(uint32_t a, uint32_t b)
{
    return mod_mul(a, INVERSE_MUL_MOD_251[mod_reduce(b)]);
}

