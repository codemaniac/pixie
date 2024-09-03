#include "include/utils.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint8_t utils_str_indexof(const char* str, const char ch) {
    char* index;

    index = strchr(str, ch);

    if (index == NULL)
    {
        return -1;
    }

    return (uint8_t) (index - str);
}

void utils_display_binary32(const int32_t x) {
    for (int8_t i = 31; i >= 0; i--)
    {
        putchar((x & (1 << i)) ? '1' : '0');
        if (i % 4 == 0)
        {
            putchar(' ');
        }
    }
    putchar('\n');
}


void utils_display_binary64(const int64_t x) {
    uint8_t sq;

    for (int8_t r = 7; r >= 0; r--)
    {
        for (int8_t f = 0; f < 8; f++)
        {
            sq = (8 * r + f);
            putchar((x & (1ULL << sq)) ? '1' : '0');
            putchar(' ');
        }
        putchar('\n');
    }
    putchar('\n');
}

uint8_t utils_bit_count1s(int64_t n) {
    uint8_t count = 0;
    while (n)
    {
        count++;
        n &= n - 1;
    }
    return count;
}

uint64_t utils_bit_pop(const int64_t n, const uint8_t k) {
    int64_t b = 1ULL << k;

    if (n & b)
    {
        return n ^ b;
    }

    return 0;
}
