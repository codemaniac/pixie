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
  HashTable* table, Position* position, uint8_t depth, HashFlag flag, int32_t value) {

    uint64_t index = position->hash % table->size;
    assert(index >= 0 && index <= table->size);

    HashTableEntry entry = {
      .hash = position->hash, .depth = depth, .flag = flag, .value = value, .is_valid = true};

    table->contents[index] = entry;
}

static HashTableEntry _hashtable_probe(HashTable* table, Position* position) {
    uint64_t index = position->hash % table->size;
    assert(index >= 0 && index <= table->size);

    if (table->contents[index].hash == position->hash)
        return table->contents[index];

    HashTableEntry empty_entry = {
      .hash = 0ULL, .depth = 0, .value = -SEARCH_SCORE_MAX, .is_valid = false};
    return empty_entry;
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
    return alpha;
}

static int32_t _search_negamax(Position*   position,
                               uint8_t     depth,
                               int32_t     alpha,
                               int32_t     beta,
                               SearchInfo* info,
                               HashTable*  table,
                               Move*       best_move) {

    info->nodes++;

    int32_t alpha_orig = alpha;

    HashTableEntry entry = _hashtable_probe(table, position);
    if (entry.is_valid && entry.depth >= depth)
    {
        if (entry.flag == EXACT)
            return entry.value;
        else if (entry.flag == LOWERBOUND)
            alpha = alpha > entry.value ? alpha : entry.value;
        else if (entry.flag == UPPERBOUND)
            beta = beta < entry.value ? beta : entry.value;

        if (alpha >= beta)
            return entry.value;
    }

    if (position_is_repeated(position) || position->half_move_clock >= 100)
        return 0;
    if (position_is_in_check(position))
        depth++;
    if (depth == 0)
        return _quiescence(position, alpha, beta, info);

    Move    best_move_so_far = {.move_id = 0, .score = -1};
    int32_t value            = -SEARCH_SCORE_MAX;

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
        int32_t score =
          -_search_negamax(position, depth - 1, -beta, -alpha, info, table, best_move);
        move_undo(position);
        value = score > value ? score : value;
        if (value > alpha)
        {
            alpha = value;
            if (position->ply_count == 0)
                best_move_so_far = move;
        }
        if (alpha >= beta)
            break;
    }

    if (legal_moves_count == 0)
    {
        if (position_is_in_check(position))
            return -SEARCH_SCORE_MAX + position->ply_count;
        else
            return 0;
    }

    if (alpha != alpha_orig)
        *best_move = best_move_so_far;

    HashFlag flag;
    if (value <= alpha_orig)
        flag = UPPERBOUND;
    else if (value >= beta)
        flag = LOWERBOUND;
    else
        flag = EXACT;
    _hashtable_store(table, position, depth, flag, value);

    return value;
}

void hashtable_init(HashTable* table) {
    table->size = 0x500000 / sizeof(HashTableEntry);
    table->size -= 2;

    HashTableEntry entries[table->size];
    HashTableEntry empty_entry = {
      .hash = 0ULL, .depth = 0, .value = -SEARCH_SCORE_MAX, .is_valid = false};
    for (uint64_t i = 0; i < table->size; i++)
        entries[i] = empty_entry;
    table->contents = entries;
}

int32_t search(Position* position, SearchInfo* info, Move* best_move) {
    HashTable table;
    hashtable_init(&table);

    return _search_negamax(position, info->depth, -SEARCH_SCORE_MAX, SEARCH_SCORE_MAX, info, &table,
                           best_move);
}
