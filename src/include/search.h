#ifndef SEARCH_H
#define SEARCH_H

#include "chess.h"
#include <stdint.h>

int32_t search(Position* position, uint8_t depth);

#endif
