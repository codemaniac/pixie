#ifndef SEARCH_H
#define SEARCH_H

#include "position.h"
#include "tt.h"
#include <cstdint>
#include <memory>

#define SEARCH_SCORE_MAX 50000
#define SEARCH_MATE_VALUE 49000
#define SEARCH_MATE_SCORE 48000
#define SEARCH_DEPTH_MAX 64

struct SearchInfo {
    int32_t  depth;
    uint64_t starttime;
    uint64_t stoptime;
    bool     timeset;
    uint8_t  movestogo;
    uint64_t nodes;
    bool     stopped;
    bool     use_iterative;
    bool     use_uci;
};

int32_t
search(std::unique_ptr<Position>&, std::unique_ptr<TranspositionTable>& table, SearchInfo* info);

#endif
