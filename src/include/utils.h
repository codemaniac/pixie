#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

uint8_t  utils_bit_bitscan_forward(uint64_t* x);
uint8_t  utils_bit_count1s(int64_t n);
uint64_t utils_bit_pop(const int64_t n, const uint8_t k);
uint8_t  utils_str_indexof(const char* str, const char ch);
uint64_t utils_time_curr_time_ms(void);
void     utils_bin_display32(const int32_t x);
void     utils_bin_display64(const int64_t x);

#endif
