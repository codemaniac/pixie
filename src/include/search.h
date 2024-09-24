#ifndef SEARCH_H
#define SEARCH_H

#include "chess.h"
#include <stdbool.h>
#include <stdint.h>

#define SEARCH_SCORE_MAX 999999
#define SEARCH_DEPTH_MAX 64
#define HASHVALUNKNOWN 989898
#define IS_MATE (SEARCH_SCORE_MAX - SEARCH_DEPTH_MAX)

typedef enum {
    HASHFINVALID,
    HASHFEXACT,
    HASHFALPHA,
    HASHFBETA,
} NodeType;

typedef struct {
    uint64_t hash;
    uint8_t  depth;
    NodeType flag;
    int32_t  value;
} HashTableEntry;

typedef struct {
    HashTableEntry* contents;
    uint64_t        size;
} HashTable;

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

void    hashtable_init(HashTable* table);
int32_t search(Position* position, SearchInfo* info, PVLine* pv_line);

#endif
