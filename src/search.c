#include "include/search.h"
#include "include/chess.h"
#include "include/eval.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define INFINITY 999999

static int32_t
_search_negamax(Position* position, uint8_t depth, int32_t alpha, int32_t beta, Move* best_move) {

    if (position_is_in_checkmate(position))
        return -INFINITY + position->ply_count;
    if (position_is_in_stalemate(position))
        return 0;
    if (position_is_repeated(position) || position->half_move_clock >= 100)
        return 0;
    if (depth == 0)
        return eval_position(position);

    Move    best_move_so_far = {.move_id = 0};
    int32_t old_alpha        = alpha;
    int32_t score            = -INFINITY;
    bool    is_beta_cutoff   = false;

    MoveList*      moves = movegen_pseudo_legal(position);
    MovesListNode *m, *t;
    bool           is_valid_move = false;

    m = (MovesListNode*) moves->head->next;

    while (m != NULL)
    {
        is_valid_move = move_do(position, m->move);

        if (!is_valid_move || !position_is_valid(position))
        {
            move_undo(position);
            t = m;
            m = (MovesListNode*) m->next;
            free(t);
            t = NULL;
            continue;
        }

        score = -_search_negamax(position, depth - 1, -beta, -alpha, best_move);

        move_undo(position);

        if (score > alpha)
        {
            alpha            = score;
            best_move_so_far = m->move;
        }

        t = m;
        m = (MovesListNode*) m->next;
        free(t);
        t = NULL;

        if (score >= beta)
        {
            is_beta_cutoff = true;
            break;
        }
    }

    while (m != NULL)
    {
        t = m;
        m = (MovesListNode*) m->next;
        free(t);
        t = NULL;
    }

    if (is_beta_cutoff)
        return beta;

    if (old_alpha != alpha)
        *best_move = best_move_so_far;

    return alpha;
}

int32_t search(Position* position, const uint8_t depth, Move* best_move) {
    return _search_negamax(position, depth, -INFINITY, INFINITY, best_move);
}
