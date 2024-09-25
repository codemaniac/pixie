#ifndef SEARCH_H
#define SEARCH_H

#include "chess.h"
#include <stdbool.h>
#include <stdint.h>

#define SEARCH_SCORE_MAX 999999
#define SEARCH_DEPTH_MAX 64

typedef struct {
    uint8_t  depth;
    uint64_t starttime;
    uint64_t stoptime;
    bool     timeset;
    uint32_t movestogo;
    uint64_t nodes;
    bool     quit;
    bool     stopped;
} SearchInfo;

int32_t search(Position* position, SearchInfo* info, const bool iterative, const bool is_uci);

#endif
