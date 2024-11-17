#ifndef SEARCH_H
#define SEARCH_H

#include "position.h"
#include "threadpool.h"
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
    bool     stopped;
    bool     use_iterative;
    bool     use_uci;
};

std::pair<int32_t, uint64_t> search(std::unique_ptr<Position>&           position,
                                    std::unique_ptr<TranspositionTable>& table,
                                    std::unique_ptr<ThreadPool>&         pool,
                                    SearchInfo*                          info);

#endif
