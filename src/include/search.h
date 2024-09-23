#ifndef SEARCH_H
#define SEARCH_H

#include "chess.h"
#include <stdbool.h>
#include <stdint.h>

#define SEARCH_SCORE_MAX 999999
#define SEARCH_MATE_SCORE 899999
#define SEARCH_DEPTH_MAX 64

typedef struct {
    int  count;
    Move moves[SEARCH_DEPTH_MAX];
} PVLine;

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

int32_t search(Position* position, SearchInfo* info, PVLine* pv_line);

#endif
