#ifndef HASHKEY_H
#define HASHKEY_H

#include "chess.h"
#include <inttypes.h>

#define HASH_IDX(p) ((p > 8) ? p - 3 : p - 1)

extern const unsigned long long HASHTABLE[12][64];
extern const unsigned long long HASH_WKCA;
extern const unsigned long long HASH_WQCA;
extern const unsigned long long HASH_BKCA;
extern const unsigned long long HASH_BQCA;
extern const unsigned long long HASH_BLACK_TO_MOVE;

unsigned long long hashkey_position(const Position* Position);

#endif
