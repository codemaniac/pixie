#ifndef PERFT_H
#define PERFT_H

#include "game.h"
#include <stdint.h>

uint64_t perft(Game* game, const uint8_t depth);

#endif
