#include "include/bitscan.h"

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

uint8_t bitscan_forward(uint64_t* x) {
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
