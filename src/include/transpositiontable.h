#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H

#include "chess.h"

typedef enum {
    NONE,
    LOWERBOUND,
    UPPERBOUND,
    EXACT,
} TTFlag;

typedef struct {
    unsigned long long hash;
    uint8_t            depth;
    TTFlag             flag;
    int32_t            value;
    Move               move;
    bool               is_valid;
} TTEntry;

typedef struct {
    TTEntry*           contents;
    unsigned long long size;
} TranspositionTable;

void hashtable_init(TranspositionTable* table);
void hashtable_clear(TranspositionTable* table);
void hashtable_store(TranspositionTable* table,
                     const Position*     position,
                     const uint8_t       depth,
                     const TTFlag        flag,
                     int32_t             value,
                     Move                move);
bool hashtable_probe(TranspositionTable* table, const Position* position, TTEntry* entry);

#endif
