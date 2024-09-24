#include "include/search.h"
#include "include/chess.h"
#include "include/eval.h"
#include "include/utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void _hashtable_store(
  HashTable* table, Position* position, uint8_t depth, NodeType flag, int32_t value) {

    uint64_t index = position->hash % table->size;
    assert(index >= 0 && index <= table->size);

    if (value > IS_MATE)
        value += position->ply_count;
    else if (value < -IS_MATE)
        value -= position->ply_count;

    HashTableEntry entry = {.hash = position->hash, .depth = depth, .flag = flag, .value = value};

    table->contents[index] = entry;
}

static int32_t
_hashtable_probe(HashTable* table, Position* position, uint8_t depth, int32_t alpha, int32_t beta) {
    uint64_t index = position->hash % table->size;
    assert(index >= 0 && index <= table->size);

    if (table->contents[index].hash == position->hash)
    {
        if (table->contents[index].depth >= depth)
        {
            if (table->contents[index].flag == HASHFEXACT)
                return table->contents[index].value;
            if (table->contents[index].flag == HASHFALPHA && table->contents[index].value <= alpha)
                return alpha;
            if (table->contents[index].flag == HASHFBETA && table->contents[index].value >= beta)
                return beta;
        }
    }
    return HASHVALUNKNOWN;
}

static int32_t _quiescence(Position* position, int32_t alpha, int32_t beta, SearchInfo* info) {

    info->nodes++;

    int32_t evaluation = eval_position(position);

    if (position->ply_count >= SEARCH_DEPTH_MAX - 1)
        return evaluation;

    if (position_is_repeated(position) || position->half_move_clock >= 100)
        return 0;

    if (evaluation >= beta)
        return beta;
    if (evaluation > alpha)
        alpha = evaluation;

    MoveList moves;
    Move     move;
    bool     is_valid_move = false;

    movegen_pseudo_legal_captures(position, &moves);

    while (movegen_dequeue_move(&moves, &move))
    {
        is_valid_move = move_do(position, move);
        if (!is_valid_move || !position_is_valid(position))
        {
            move_undo(position);
            continue;
        }
        int32_t score = -_quiescence(position, -beta, -alpha, info);
        move_undo(position);

        if (score >= beta)
            return beta;

        if (score > alpha)
            alpha = score;
    }

    // node fails low
    return alpha;
}

static int32_t _search_negamax(Position*   position,
                               uint8_t     depth,
                               int32_t     alpha,
                               int32_t     beta,
                               SearchInfo* info,
                               HashTable*  table,
                               PVLine*     pv_line) {

    info->nodes++;

    int32_t value = -SEARCH_SCORE_MAX;

    if ((value = _hashtable_probe(table, position, depth, alpha, beta)) != HASHVALUNKNOWN)
        return value;

    if (position_is_repeated(position) || position->half_move_clock >= 100)
        return 0;
    if (position_is_in_check(position))
        depth++;
    if (depth == 0)
    {
        pv_line->count = 0;
        return _quiescence(position, alpha, beta, info);
    }

    PVLine line;

    MoveList moves;
    Move     move;
    uint32_t legal_moves_count = 0;
    bool     is_valid_move     = false;

    movegen_pseudo_legal_all(position, &moves);

    while (movegen_dequeue_move(&moves, &move))
    {
        is_valid_move = move_do(position, move);
        if (!is_valid_move || !position_is_valid(position))
        {
            move_undo(position);
            continue;
        }
        legal_moves_count++;
        value = -_search_negamax(position, depth - 1, -beta, -alpha, info, table, &line);
        move_undo(position);
        if (value >= beta)
        {
            _hashtable_store(table, position, depth, HASHFBETA, value);
            return beta;
        }
        if (value > alpha)
        {
            alpha = value;
            _hashtable_store(table, position, depth, HASHFEXACT, value);
            pv_line->moves[0] = move;
            memcpy(pv_line->moves + 1, line.moves, line.count * sizeof(Move));
            pv_line->count = line.count + 1;
        }
    }

    if (legal_moves_count == 0)
    {
        if (position_is_in_check(position))
            return -SEARCH_SCORE_MAX + position->ply_count;
        else
            return 0;
    }

    _hashtable_store(table, position, depth, HASHFALPHA, alpha);
    return alpha;
}

void hashtable_init(HashTable* table) {
    table->size = 0x500000 / sizeof(HashTableEntry);
    table->size -= 2;

    HashTableEntry entries[table->size];
    HashTableEntry empty_entry = {
      .hash = 0ULL, .depth = 0, .flag = HASHFINVALID, .value = -SEARCH_SCORE_MAX};
    for (uint64_t i = 0; i < table->size; i++)
        entries[i] = empty_entry;
    table->contents = entries;
}

int32_t search(Position* position, SearchInfo* info, PVLine* pv_line) {
    HashTable table;
    hashtable_init(&table);

    return _search_negamax(position, info->depth, -SEARCH_SCORE_MAX, SEARCH_SCORE_MAX, info, &table,
                           pv_line);
}
