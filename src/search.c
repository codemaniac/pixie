#include "include/search.h"
#include "include/chess.h"
#include "include/eval.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static int32_t _search_quiesce(Position* position, int32_t alpha, int32_t beta) {
    int32_t score = eval_position(position);

    if (score >= beta)
        return beta;
    if (alpha < score)
        alpha = score;

    MoveList*      moves = movegen_pseudo_legal(position);
    MovesListNode *m, *t;
    bool           is_valid_move = false;

    m = (MovesListNode*) moves->head->next;

    while (m != NULL)
    {
        if (m->move.type == MOVE_TYPE_QUITE)
        {
            t = m;
            m = (MovesListNode*) m->next;
            free(t);
            continue;
        }

        is_valid_move = move_do(position, m->move);
        if (!is_valid_move || !position_is_valid(position))
        {
            move_undo(position);
            t = m;
            m = (MovesListNode*) m->next;
            free(t);
            continue;
        }
        score = -_search_quiesce(position, -beta, -alpha);
        move_undo(position);
        if (score >= beta)
            return beta;
        if (score > alpha)
            alpha = score;

        t = m;
        m = (MovesListNode*) m->next;
        free(t);
    }

    return alpha;
}

static int32_t _search_alphabeta(Position* position, uint8_t depth, int32_t alpha, int32_t beta) {
    if (depth == 0)
        return _search_quiesce(position, alpha, beta);

    int32_t best_value = -9999;
    int32_t score      = 0;

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
            continue;
        }

        score = -_search_alphabeta(position, depth - 1, -beta, -alpha);

        move_undo(position);

        if (score > best_value)
        {
            best_value = score;
            if (score > alpha)
                alpha = score;
        }
        if (score >= beta)
            return best_value;

        t = m;
        m = (MovesListNode*) m->next;
        free(t);
    }

    return best_value;
}

int32_t search(Position* position, uint8_t depth) {
    return _search_alphabeta(position, depth, -9999, 9999);
}
