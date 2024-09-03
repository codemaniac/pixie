#ifndef PERFT_H
#define PERFT_H

#include "chess.h"
#include <stdint.h>

uint64_t perft(Position* position, const uint8_t depth);
void     divide(Position* position, const uint8_t depth);

#endif
