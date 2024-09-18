#include "include/utils.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

static const uint8_t tab64[64] = {63, 0,  58, 1,  59, 47, 53, 2,  60, 39, 48, 27, 54, 33, 42, 3,
                                  61, 51, 37, 40, 49, 18, 28, 20, 55, 30, 34, 11, 43, 14, 22, 4,
                                  62, 57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10, 13, 21,
                                  56, 45, 25, 31, 35, 16, 9,  12, 44, 24, 15, 8,  23, 7,  6,  5};

static uint8_t log2_64(uint64_t value) {
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    value |= value >> 32;
    return tab64[((uint64_t) ((value - (value >> 1)) * 0x07EDD5E59A4E28C2)) >> 58];
}

uint8_t utils_bit_bitscan_forward(uint64_t* x) {
    uint8_t position;
#ifdef __GNUC__
    position = __builtin_ctzll(*x);
#else
    uint64_t ls1b = *x & -*x;
    position      = log2_64(ls1b);
#endif
    *x &= *x - 1;
    return position;
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

uint8_t utils_str_indexof(const char* str, const char ch) {
    char* index;

    index = strchr(str, ch);

    if (index == NULL)
    {
        return -1;
    }

    return (uint8_t) (index - str);
}

char* utils_str_trimwhitespace(char* str) {
    char* end;

    // Trim leading space
    while (isspace((unsigned char) *str))
        str++;

    if (*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end))
        end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

uint64_t utils_time_curr_time_ms(void) {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (uint64_t) (t.tv_sec * 1000) + t.tv_usec / 1000;
}

void utils_bin_display32(const int32_t x) {
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

void utils_bin_display64(const int64_t x) {
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
