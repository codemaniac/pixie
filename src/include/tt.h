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

struct TTEntry {
    uint64_t hash;
    uint8_t  depth;
    uint8_t  age;
    TTFlag   flag;
    int32_t  value;
    Move     move;
    bool     is_valid;

    TTEntry() {
        this->hash     = 0ULL;
        this->depth    = 0;
        this->age      = 0;
        this->flag     = NONE;
        this->value    = 0;
        this->move     = Move();
        this->is_valid = false;
    }

    TTEntry(const uint64_t hash,
            const uint8_t  depth,
            const TTFlag   flag,
            const int32_t  value,
            const Move     move) {
        this->hash     = hash;
        this->depth    = depth;
        this->flag     = flag;
        this->value    = value;
        this->move     = move;
        this->is_valid = true;
    }
};

class TranspositionTable {
   private:
    std::unique_ptr<TTEntry[]> entries;
    size_t                     size;

   public:
    TranspositionTable(const uint8_t size_in_mb);
    ~TranspositionTable();
    void     clear();
    void     store(std::unique_ptr<Position>& position,
                   const uint8_t              depth,
                   const TTFlag               flag,
                   int32_t                    value,
                   const Move                 move);
    bool     probe(std::unique_ptr<Position>& position, TTEntry* entry) const;
    uint64_t get_size() const;
};

#endif