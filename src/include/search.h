#ifndef SEARCH_H
#define SEARCH_H

#include "chess.h"
#include <stdint.h>

int32_t search(Position* position, const uint8_t depth, Move* best_move);

#endif
