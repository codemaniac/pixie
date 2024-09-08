#ifndef SEARCH_H
#define SEARCH_H

#include "chess.h"
#include <stdint.h>

typedef struct {
    uint64_t hash;
    Move     move;
} PVEntry;

typedef struct {
    PVEntry* contents;
    uint64_t size;
} PVTable;

PVTable* search_init_pv_table(void);
int32_t  search(Position* position, uint8_t depth);

#endif
