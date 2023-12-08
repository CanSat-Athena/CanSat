#include <stdio.h>
#include "pico/stdlib.h"

int main()
{
    stdio_init_all();

    puts("Hello, world!");

    while(true);
    return 0;
}