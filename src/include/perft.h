#ifndef PERFT_H
#define PERFT_H

#include "position.h"
#include <stdint.h>

uint64_t perft(Position* position, const uint8_t depth);

#endif
