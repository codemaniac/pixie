#ifndef HASHKEY_H
#define HASHKEY_H

#include "position.h"
#include <stdint.h>
#include <stdlib.h>

#define PIECE_TO_HASH_ID(p) (p - 1 - ((p & 0x8) / 0x8 * 2))
#define RAND_64 \
    (((uint64_t) rand()) + ((uint64_t) rand() << 15) \
     + ((uint64_t) rand() << 30) + ((uint64_t) rand() << 45) \
     + (((uint64_t) rand() & 0xF) << 60))

void     hashkey_init(void);
uint64_t hashkey_position(Position* Position);

#endif
