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
void     search_add_pv_entry(PVTable* table, const Position* position, const Move move);
Move     search_probe_pv_for_position(PVTable* table, const Position* position);
int32_t  search(Position* position, uint8_t depth);

#endif
