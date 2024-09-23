#include "include/search.h"
#include "include/chess.h"
#include "include/eval.h"
#include "include/utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
                               PVLine*     pv_line) {

    if (position_is_repeated(position) || position->half_move_clock >= 100)
        return 0;
    if (position_is_in_check(position))
        depth++;
    if (depth == 0)
    {
        pv_line->count = 0;
        return _quiescence(position, alpha, beta, info);
    }

    info->nodes++;

    PVLine   line;
    Move     best_move_so_far = {.move_id = 0, .score = -1};
    int32_t  old_alpha        = alpha;
    int32_t  score            = -SEARCH_SCORE_MAX;
    uint32_t legal            = 0;

    MoveList moves;
    Move     move;
    bool     is_valid_move = false;

    movegen_pseudo_legal_all(position, &moves);

    while (movegen_dequeue_move(&moves, &move))
    {
        is_valid_move = move_do(position, move);
        if (!is_valid_move || !position_is_valid(position))
        {
            move_undo(position);
            continue;
        }
        legal++;
        score = -_search_negamax(position, depth - 1, -beta, -alpha, info, &line);
        move_undo(position);

        if (score >= beta)
        {
            // Node fails high
            return beta;
        }

        // Better move found
        if (score > alpha)
        {
            // PV node
            alpha             = score;
            pv_line->moves[0] = move;
            memcpy(pv_line->moves + 1, line.moves, line.count * sizeof(Move));
            pv_line->count = line.count + 1;
        }
    }

    if (legal == 0)
    {
        if (position_is_in_check(position))
            return -SEARCH_MATE_SCORE + position->ply_count;
        else
            return 0;
    }

    // node fails low
    return alpha;
}

int32_t search(Position* position, SearchInfo* info, PVLine* pv_line) {
    return _search_negamax(position, info->depth, -SEARCH_SCORE_MAX, SEARCH_SCORE_MAX, info,
                           pv_line);
}
