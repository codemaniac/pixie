#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H

#include "move.h"
#include "position.h"
#include <cstdint>
#include <memory>
#include <vector>

enum TTFlag : uint8_t {
    NONE,
    LOWERBOUND,
    UPPERBOUND,
    EXACT,
};

struct TTData {
    uint8_t depth;  // 6 bits
    TTFlag  flag;   // 2 bits
    int32_t value;  // 16 bits + 1 bit for overflow
    Move    move;   // 36 bits
    bool    is_valid;

    TTData() {
        this->depth    = 0;
        this->flag     = NONE;
        this->value    = 0;
        this->move     = Move();
        this->is_valid = false;
    }
};

struct TTEntry {
    uint64_t key;
    uint64_t data;
    uint8_t  age;

    TTEntry() {
        this->key  = 0ULL;
        this->data = 0ULL;
        this->age  = 0;
    }
};

class TranspositionTable {
   private:
    std::unique_ptr<TTEntry[]> entries;
    size_t                     size;
    uint32_t                   current_age;

   public:
#ifdef DEBUG
    int new_writes_empty;
    int new_writes_age;
    int new_writes_depth;
#endif

    TranspositionTable(const uint8_t size_in_mb);
    ~TranspositionTable();
    void clear();
    void reset_for_search();
#ifdef DEBUG
    void reset_counters();
#endif
    void     store(std::unique_ptr<Position>& position,
                   const uint8_t              depth,
                   const TTFlag               flag,
                   int32_t                    value,
                   const Move                 move);
    bool     probe(std::unique_ptr<Position>& position, TTData* ttdata) const;
    uint64_t get_size() const;
};

#endif
