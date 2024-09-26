#ifndef PERFT_H
#define PERFT_H

#include "chess.h"
#include <inttypes.h>

uint64_t perft(Position* position, const uint8_t depth);
void     divide(Position* position, const uint8_t depth);

#endif
