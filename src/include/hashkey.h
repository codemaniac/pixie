#ifndef HASHKEY_H
#define HASHKEY_H

#include "chess.h"
#include <stdint.h>

#define HASH_IDX(p) ((p > 8) ? p - 3 : p - 1)

const uint64_t HASHTABLE[12][64];

uint64_t hashkey_position(Position* Position);

#endif
