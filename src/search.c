#include "include/search.h"
#include "include/chess.h"
#include "include/eval.h"
#include "include/transpositiontable.h"
#include "include/utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int32_t pv_length[SEARCH_DEPTH_MAX];
int32_t pv_table[SEARCH_DEPTH_MAX][SEARCH_DEPTH_MAX];

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

static int32_t _search_negamax(Position*           position,
                               uint8_t             depth,
                               int32_t             alpha,
                               int32_t             beta,
                               SearchInfo*         info,
                               TranspositionTable* table) {

    info->nodes++;
    pv_length[position->ply_count] = position->ply_count;

    int32_t alpha_orig = alpha;

    TTEntry entry;
    bool    hashtable_probe_status = hashtable_probe(table, position, &entry);

    if (hashtable_probe_status && entry.is_valid && entry.depth >= depth)
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

    if (position->ply_count >= SEARCH_DEPTH_MAX - 1)
        return eval_position(position);
    if (position_is_repeated(position) || position->half_move_clock >= 100)
        return 0;
    if (position_is_in_check(position))
        depth++;
    if (depth == 0)
        return _quiescence(position, alpha, beta, info);

    int32_t value = -SEARCH_SCORE_MAX;

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
        int32_t score = -_search_negamax(position, depth - 1, -beta, -alpha, info, table);
        move_undo(position);
        value = score > value ? score : value;
        if (value > alpha)
        {
            alpha = value;
            // Set PV
            const uint8_t ply  = position->ply_count;
            pv_table[ply][ply] = move.move_id;
            for (int next_ply = ply + 1; next_ply < pv_length[ply + 1]; next_ply++)
                pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];
            pv_length[ply] = pv_length[ply + 1];
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

    TTFlag flag;
    if (value <= alpha_orig)
        flag = UPPERBOUND;
    else if (value >= beta)
        flag = LOWERBOUND;
    else
        flag = EXACT;
    hashtable_store(table, position, depth, flag, value);

    return value;
}

int32_t search(Position* position, SearchInfo* info, const bool iterative, const bool is_uci) {
    TranspositionTable table;
    hashtable_init(&table);

    memset(pv_length, 0, sizeof(pv_length));
    memset(pv_table, 0, sizeof(pv_table));

    int32_t score;
    Move    pv_move;
    char    move_str[10];

    uint8_t currdepth = 1;

    if (iterative)
    {

        for (currdepth = 1; currdepth <= info->depth; currdepth++)
        {
            score = _search_negamax(position, currdepth, -SEARCH_SCORE_MAX, SEARCH_SCORE_MAX, info,
                                    &table);

            if (is_uci)
            {
                printf("info score cp %d depth %d nodes %llu pv ", score, currdepth, info->nodes);

                uint8_t pv_length_to_show = (pv_length[0] > 5) ? 5 : pv_length[0];

                for (uint8_t count = 0; count < pv_length_to_show; count++)
                {
                    pv_move.move_id = pv_table[0][count];
                    move_to_str(move_str, pv_move);
                    printf("%s ", move_str);
                }
                printf("\n");
            }
        }
    }
    else
    {
        score =
          _search_negamax(position, info->depth, -SEARCH_SCORE_MAX, SEARCH_SCORE_MAX, info, &table);
    }

    if (is_uci)
    {
        pv_move.move_id = pv_table[0][0];
        move_to_str(move_str, pv_move);
        printf("bestmove %s\n", move_str);
    }

    free(table.contents);
    table.contents = NULL;

    return score;
}
