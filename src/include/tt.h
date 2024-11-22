#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H

#include "move.h"
#include "position.h"
#include <cstdint>
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
#ifdef DEBUG
    int tid;
#endif

    TTData() {
        this->depth    = 0;
        this->flag     = NONE;
        this->value    = 0;
        this->move     = Move();
        this->is_valid = false;
#ifdef DEBUG
        this->tid = 0;
#endif
    }
};

struct TTEntry {
    uint64_t key;
    uint64_t data;
    uint8_t  age;
#ifdef DEBUG
    int tid;
#endif

    TTEntry() {
        this->key  = 0ULL;
        this->data = 0ULL;
        this->age  = 0;
#ifdef DEBUG
        this->tid = 0;
#endif
    }
};

class TranspositionTable {
   private:
    std::vector<TTEntry> entries;
    size_t               size;
    uint32_t             current_age;

   public:
#ifdef DEBUG
    int new_writes_empty;
    int new_writes_age;
    int new_writes_depth;
    int crossthread_hit;
#endif

    TranspositionTable(const uint32_t size_in_mb);
    ~TranspositionTable();
    void clear();
    void reset_for_search();
#ifdef DEBUG
    void reset_counters();
#endif
    void     store(const Position& position,
                   const uint8_t   depth,
                   const TTFlag    flag,
                   int32_t         value,
                   const Move      move,
                   const int       tid);
    bool     probe(const Position& position, TTData* ttdata, const int tid);
    uint64_t get_size() const;
};

#endif
