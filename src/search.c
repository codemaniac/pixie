#include "include/search.h"
#include "include/chess.h"
#include "include/eval.h"
#include "include/utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static void _search_check_up(SearchInfo* info) {
    if (info->timeset && (utils_time_curr_time_ms() >= info->stoptime))
        info->stopped = true;
}

static int32_t _search_negamax(Position*   position,
                               uint8_t     depth,
                               int32_t     alpha,
                               int32_t     beta,
                               SearchInfo* info,
                               Move*       best_move) {

    if (position_is_repeated(position) || position->half_move_clock >= 100)
        return 0;
    if (position_is_in_check(position))
        depth++;
    if (depth == 0)
        return eval_position(position);

    // if ((info->nodes & 2047) == 0)
    //     _search_check_up(info);

    info->nodes++;

    Move     best_move_so_far = {.move_id = 0};
    int32_t  old_alpha        = alpha;
    int32_t  score            = -SEARCH_SCORE_MAX;
    bool     is_beta_cutoff   = false;
    uint32_t legal            = 0;

    MoveList moves;
    Move     move;
    bool     is_valid_move = false;

    movegen_pseudo_legal(position, &moves);

    while (movegen_dequeue_move(&moves, &move))
    {
        is_valid_move = move_do(position, move);
        if (!is_valid_move || !position_is_valid(position))
        {
            move_undo(position);
            continue;
        }
        legal++;
        score = -_search_negamax(position, depth - 1, -beta, -alpha, info, best_move);
        move_undo(position);

        // if (info->stopped)
        //     break;

        if (score > alpha)
        {
            alpha            = score;
            best_move_so_far = move;
        }
        if (score >= beta)
        {
            is_beta_cutoff = true;
            break;
        }
    }

    if (info->stopped)
        return 0;

    if (is_beta_cutoff)
        return beta;

    if (legal == 0)
    {
        if (position_is_in_check(position))
            return -SEARCH_SCORE_MAX + position->ply_count;
        else
            return 0;
    }

    if (old_alpha != alpha)
        *best_move = best_move_so_far;

    return alpha;
}

int32_t search(Position* position, SearchInfo* info, Move* best_move) {
    return _search_negamax(position, info->depth, -SEARCH_SCORE_MAX, SEARCH_SCORE_MAX, info,
                           best_move);
}
