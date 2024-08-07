#include "include/utils.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int str_indexof(const char* str, const char ch) {
    char* index;

    index = strchr(str, ch);

    if (index == NULL)
    {
        return -1;
    }

    return (int) (index - str);
}

void display_binary(const uint32_t number) {
    for (int8_t i = 31; i >= 0; i--)
    {
        putchar((number & (1 << i)) ? '1' : '0');
        if (i % 8 == 0)
        {
            putchar(' ');  // Optional: add a space every 8 bits for readability
        }
    }
    putchar('\n');
}
