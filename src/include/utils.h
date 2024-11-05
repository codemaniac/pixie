#ifndef UTILS_H
#define UTILS_H

#include <bit>
#include <cstdint>

int      utils_bit_bitscan_forward(uint64_t* x);
uint8_t  utils_bit_count1s(const uint64_t n);
void     utils_bb_display(const uint64_t x);
uint64_t utils_get_current_time_in_milliseconds();
uint64_t utils_random_uint64();

#endif
