#include "include/tt.h"
#include "include/move.h"
#include "include/position.h"
#include <cassert>
#include <cstdint>

#define INF_BOUND 52000
#define SEARCH_MATE_SCORE 48000

static constexpr uint64_t
FOLD_DATA(const uint8_t depth, const TTFlag flag, const int32_t value, const uint64_t move_id) {
    return ((move_id << 25) | ((value + INF_BOUND) << 8) | (flag << 6) | depth);
}
static constexpr uint8_t GET_DEPTH(const uint64_t data) { return (data & 0x3F); }
static constexpr TTFlag  GET_FLAG(const uint64_t data) {
    return (static_cast<TTFlag>((data >> 6) & 0x3));
}
static constexpr int32_t GET_VALUE(const uint64_t data) {
    return (((data >> 8) & 0x1FFFF) - INF_BOUND);
}
static constexpr uint32_t GET_MOVE_ID(const uint64_t data) { return (data >> 25); }

TranspositionTable::TranspositionTable(const uint8_t size_in_mb) {
    this->size = (size_in_mb * 0x100000) / sizeof(TTEntry);
    this->size -= sizeof(TTEntry);
    this->entries.clear();
    this->entries.resize(this->size);
    this->entries.shrink_to_fit();
    this->clear();
}

TranspositionTable::~TranspositionTable() {}

void TranspositionTable::clear() {
    for (size_t i = 0; i < this->size; i++)
        this->entries[i] = TTEntry();
    this->current_age = 0;
}

void TranspositionTable::reset_for_search() { this->current_age++; }

#ifdef DEBUG
void TranspositionTable::reset_counters() {
    this->new_writes_empty = 0;
    this->new_writes_age   = 0;
    this->new_writes_depth = 0;
    this->crossthread_hit  = 0;
}
#endif

void TranspositionTable::store(const Position& position,
                               const uint8_t   depth,
                               const TTFlag    flag,
                               int32_t         value,
                               const Move      move,
                               const int       tid) {
    const uint64_t hash  = position.get_hash();
    const uint64_t index = hash % this->size;
    assert(index < this->size);

    bool replace = false;

    if (this->entries[index].key == 0ULL)
    {
        replace = true;
#ifdef DEBUG
        this->new_writes_empty++;
#endif
    }
    else if (this->entries[index].age < this->current_age)
    {
        replace = true;
#ifdef DEBUG
        this->new_writes_age++;
#endif
    }
    else if (GET_DEPTH(this->entries[index].data) <= depth)
    {
        replace = true;
#ifdef DEBUG
        this->new_writes_depth++;
#endif
    }

    if (!replace)
        return;

    if (value < -SEARCH_MATE_SCORE)
        value -= position.get_ply_count();
    else if (value > SEARCH_MATE_SCORE)
        value += position.get_ply_count();

    const uint64_t data = FOLD_DATA(depth, flag, value, move.get_full_id());
    const uint64_t key  = hash ^ data;

    this->entries[index].key  = key;
    this->entries[index].data = data;
    this->entries[index].age  = this->current_age;
#ifdef DEBUG
    this->entries[index].tid = tid;
#else
    (void) tid;
#endif
}

bool TranspositionTable::probe(const Position& position, TTData* ttdata, const int tid) {
    const uint64_t hash  = position.get_hash();
    const uint64_t index = hash % this->size;
    assert(index < this->size);

    const uint64_t key  = this->entries[index].key;
    const uint64_t data = this->entries[index].data;

    if ((hash ^ data) == key)
    {
#ifdef DEBUG
        const int tttid = this->entries[index].tid;
        if (tttid != tid)
        {
            this->crossthread_hit++;
        }
#else
        (void) tid;
#endif
        ttdata->depth    = GET_DEPTH(data);
        ttdata->flag     = GET_FLAG(data);
        ttdata->value    = GET_VALUE(data);
        ttdata->move     = Move(GET_MOVE_ID(data));
        ttdata->is_valid = true;
#ifdef DEBUG
        ttdata->tid = tttid;
#endif

        return true;
    }

    return false;
}

uint64_t TranspositionTable::get_size() const { return this->size; }
