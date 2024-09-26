#ifndef SEARCH_H
#define SEARCH_H

#include "chess.h"
#include "transpositiontable.h"
#include <inttypes.h>
#include <stdbool.h>

#define SEARCH_SCORE_MAX 999999
#define SEARCH_DEPTH_MAX 64
#define SEARCH_IS_MATE (SEARCH_SCORE_MAX - SEARCH_DEPTH_MAX)

typedef struct {
    uint8_t            depth;
    unsigned long long starttime;
    unsigned long long stoptime;
    bool               timeset;
    uint32_t           movestogo;
    unsigned long long nodes;
    bool               quit;
    bool               stopped;
} SearchInfo;

int32_t search(Position*           position,
               TranspositionTable* table,
               SearchInfo*         info,
               const bool          iterative,
               const bool          is_uci);

#endif
