#include "include/utils.h"

#include <bit>
#include <chrono>
#include <cstdint>

int utils_bit_bitscan_forward(uint64_t* x) {
    int position = std::countr_zero(*x);
    *x &= *x - 1;
    return position;
}

uint8_t utils_bit_count1s(const uint64_t n) { return std::popcount(n); }

void utils_bb_display(const uint64_t x) {
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

uint64_t utils_get_current_time_in_milliseconds() {
    // Get the current time point
    auto now = std::chrono::system_clock::now();

    // Convert the time point to duration since epoch
    auto duration = now.time_since_epoch();

    // Convert duration to milliseconds
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    return milliseconds;
}

uint64_t utils_random_uint64() {
    // http://vigna.di.unimi.it/ftp/papers/xorshift.pdf

    static uint64_t seed = 1070372ULL;

    seed ^= seed >> 12;
    seed ^= seed << 25;
    seed ^= seed >> 27;

    return seed * 2685821657736338717ULL;
}
