#ifndef HASHKEY_H
#define HASHKEY_H

#include "chess.h"
#include <inttypes.h>

#define HASH_IDX(p) ((p > 8) ? p - 3 : p - 1)

extern const unsigned long long HASHTABLE[12][64];

unsigned long long hashkey_position(Position* Position);

#endif
