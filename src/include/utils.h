#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

uint8_t  utils_str_indexof(const char* str, const char ch);
void     utils_display_binary32(const int32_t x);
void     utils_display_binary64(const int64_t x);
uint8_t  utils_bit_count1s(int64_t n);
uint64_t utils_bit_pop(const int64_t n, const uint8_t k);

#endif
