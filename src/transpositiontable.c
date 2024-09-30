#include "include/transpositiontable.h"
#include "include/chess.h"
#include "include/search.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void hashtable_init(TranspositionTable* table) {
    table->size = 0x500000 / sizeof(TTEntry);
    table->size -= sizeof(TTEntry);

    table->contents = (TTEntry*) malloc(table->size * sizeof(TTEntry));
    if (table->contents == NULL)
    {
        printf("HASHTABLE MEMORY ALLOCATION FAILED!\n");
        exit(EXIT_FAILURE);
    }

    hashtable_clear(table);
}

void hashtable_clear(TranspositionTable* table) {
    const Move nomove = {.move_id = 0, .score = -1};

    TTEntry* entry;
    for (entry = table->contents; entry < table->contents + table->size; entry++)
    {
        entry->hash     = 0ULL;
        entry->depth    = 0;
        entry->flag     = NONE;
        entry->value    = -INT32_MAX;
        entry->move     = nomove;
        entry->is_valid = false;
    }
}

void hashtable_store(TranspositionTable* table,
                     const Position*     position,
                     const uint8_t       depth,
                     const TTFlag        flag,
                     int32_t             value,
                     Move                move) {

    const unsigned long long index = position->hash % table->size;
    assert(index >= 0 && index < table->size);

    if (value < -SEARCH_MATE_SCORE)
        value -= position->ply_count;
    else if (value > SEARCH_MATE_SCORE)
        value += position->ply_count;

    table->contents[index].hash     = position->hash;
    table->contents[index].depth    = depth;
    table->contents[index].flag     = flag;
    table->contents[index].value    = value;
    table->contents[index].move     = move;
    table->contents[index].is_valid = true;
}

bool hashtable_probe(TranspositionTable* table, const Position* position, TTEntry* entry) {
    unsigned long long index = position->hash % table->size;
    assert(index >= 0 && index <= table->size);

    if (table->contents[index].hash == position->hash)
    {
        entry->hash     = table->contents[index].hash;
        entry->depth    = table->contents[index].depth;
        entry->flag     = table->contents[index].flag;
        entry->value    = table->contents[index].value;
        entry->move     = table->contents[index].move;
        entry->is_valid = table->contents[index].is_valid;
        return true;
    }

    return false;
}
