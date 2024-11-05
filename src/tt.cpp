#include "include/tt.h"
#include "include/move.h"
#include "include/position.h"
#include <cassert>
#include <cstdint>

#define SEARCH_MATE_SCORE 48000

TranspositionTable::TranspositionTable(const uint8_t size_in_mb) {
    this->size = (size_in_mb * 0x100000) / sizeof(TTEntry);
    this->size -= sizeof(TTEntry);
    this->entries = std::make_unique<TTEntry[]>(this->size);
    this->clear();
}

TranspositionTable::~TranspositionTable() {}

void TranspositionTable::clear() {
    for (size_t i = 0; i < this->size; i++)
        this->entries[i] = TTEntry();
    this->current_age = 0;
}

void TranspositionTable::reset_for_search() { this->current_age++; }

void TranspositionTable::store(std::unique_ptr<Position>& position,
                               const uint8_t              depth,
                               const TTFlag               flag,
                               int32_t                    value,
                               const Move                 move) {
    const uint64_t hash  = position->get_hash();
    const uint64_t index = hash % this->size;
    assert(index < this->size);

    bool replace = false;

    if (this->entries[index].hash == 0ULL)
        replace = true;
    else if (this->entries[index].age < this->current_age)
        replace = true;
    else if (this->entries[index].depth <= depth)
        replace = true;

    if (!replace)
        return;

    if (value < -SEARCH_MATE_SCORE)
        value -= position->get_ply_count();
    else if (value > SEARCH_MATE_SCORE)
        value += position->get_ply_count();

    this->entries[index].hash     = position->get_hash();
    this->entries[index].depth    = depth;
    this->entries[index].age      = this->current_age;
    this->entries[index].flag     = flag;
    this->entries[index].value    = value;
    this->entries[index].move     = move;
    this->entries[index].is_valid = true;
}

bool TranspositionTable::probe(std::unique_ptr<Position>& position, TTEntry* entry) const {
    const uint64_t hash  = position->get_hash();
    const uint64_t index = hash % this->size;
    assert(index < this->size);

    if (this->entries[index].hash == hash)
    {
        entry->hash     = this->entries[index].hash;
        entry->depth    = this->entries[index].depth;
        entry->age      = this->entries[index].age;
        entry->flag     = this->entries[index].flag;
        entry->value    = this->entries[index].value;
        entry->move     = this->entries[index].move;
        entry->is_valid = this->entries[index].is_valid;
        return true;
    }

    return false;
}

uint64_t TranspositionTable::get_size() const { return this->size; }
