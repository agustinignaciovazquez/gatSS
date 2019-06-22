//
// Created by Agustin Vazquez on 2019-06-22.
//

#include <assert.h>
#include <mod/mod.h>
#include <stdio.h>
void setup(){

}

void cleanup(){

}
void
test_sum()
{
    uint32_t ans = mod_sum(128, 200);
    assert(ans == 77);

    printf("[test_sum] passed tests\n");
}

void
test_sub()
{
    uint32_t ans = mod_sub(128, 200);
    assert(ans == 179);

    printf("[test_sub] passed tests\n");
}

void
test_mul()
{
    uint32_t ans = mod_mul(128, 200);
    assert(ans == 249);

    printf("[test_mul] passed tests\n");
}

void
test_div()
{
    uint32_t ans = mod_div(128, 200);
    assert(ans == 91);

    printf("[test_div] passed tests\n");
}

int
main(int argc, char* argv[])
{
    setup();
    test_sum();
    test_sub();
    test_mul();
    test_div();
    cleanup();
    return 0;
}
