#include "include/transpositiontable.h"
#include "include/chess.h"
#include <assert.h>

void hashtable_init(TranspositionTable* table) {
    table->size = 0x500000 / sizeof(TTEntry);
    table->size -= 2;

    TTEntry entries[table->size];
    TTEntry empty_entry = {.hash = 0ULL, .depth = 0, .value = -INT32_MAX, .is_valid = false};
    for (uint64_t i = 0; i < table->size; i++)
        entries[i] = empty_entry;
    table->contents = entries;
}

void hashtable_store(
  TranspositionTable* table, Position* position, uint8_t depth, TTFlag flag, int32_t value) {

    uint64_t index = position->hash % table->size;
    assert(index >= 0 && index <= table->size);

    TTEntry entry = {
      .hash = position->hash, .depth = depth, .flag = flag, .value = value, .is_valid = true};

    table->contents[index] = entry;
}

TTEntry hashtable_probe(TranspositionTable* table, Position* position) {
    uint64_t index = position->hash % table->size;
    assert(index >= 0 && index <= table->size);

    if (table->contents[index].hash == position->hash)
        return table->contents[index];

    TTEntry empty_entry = {.hash = 0ULL, .depth = 0, .value = -INT32_MAX, .is_valid = false};
    return empty_entry;
}
