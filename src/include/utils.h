#ifndef UTILS_H
#define UTILS_H

#include <inttypes.h>

uint8_t            utils_bit_bitscan_forward(unsigned long long* x);
uint8_t            utils_bit_count1s(int64_t n);
unsigned long long utils_bit_pop(const int64_t n, const uint8_t k);
uint8_t            utils_str_indexof(const char* str, const char ch);
char*              utils_str_trimwhitespace(char* str);
unsigned long long utils_time_curr_time_ms(void);
void               utils_bin_display32(const int32_t x);
void               utils_bin_display64(const int64_t x);

#endif
