#ifndef HASHKEY_H
#define HASHKEY_H

#include "position.h"
#include <stdint.h>

#define PIECE_TO_HASH_ID(p) (p - 1 - (((p & 0x8) / 0x8) * 2))

uint64_t hashkey_position(Position* Position);

#endif
