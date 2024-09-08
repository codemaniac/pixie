#include "include/search.h"
#include "include/chess.h"
#include "include/eval.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static const uint64_t PVTABLESIZE = 0x100000 * 2;

PVTable* search_init_pv_table(void) {
    PVTable* table = (PVTable*) malloc(sizeof(PVTable));
    table->size    = PVTABLESIZE / sizeof(PVEntry);
    table->size    = table->size - 2;

    free(table->contents);
    table->contents = (PVEntry*) malloc(table->size * sizeof(PVEntry));

    PVEntry*   e;
    const Move nomove = {.move_id = 0};

    for (e = table->contents; e < table->contents + table->size; e++)
    {
        e->hash = 0ULL;
        e->move = nomove;
    }

    return table;
}

void search_add_pv_entry(PVTable* table, const Position* position, const Move move) {
    uint64_t index = position->hash % table->size;
    assert(index >= 0 && index < table->size);
    table->contents[index].hash = position->hash;
    table->contents[index].move = move;
}

Move search_probe_pv_for_position(PVTable* table, const Position* position) {
    uint64_t index = position->hash % table->size;
    assert(index >= 0 && index < table->size);
    if (table->contents[index].hash == position->hash)
    {
        return table->contents[index].move;
    }
    const Move nomove = {.move_id = 0};
    return nomove;
}

int32_t negamax(Position* position, uint8_t depth) {
    if (depth == 0)
        return evaluation_position_eval(position);

    int32_t max = -9999;
    int32_t score;

    MovesListNode* moves = movegen_pseudo_legal(position);
    MovesListNode *m, *t;
    bool           is_valid_move = false;

    m = (MovesListNode*) moves->next;

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
        score = -negamax(position, depth - 1);
        move_undo(position);
        if (score > max)
        {
            max = score;
        }
        t = m;
        m = (MovesListNode*) m->next;
        free(t);
    }

    return max;
}

int32_t quiesce(Position* position, int32_t alpha, int32_t beta) {
    int32_t score = evaluation_position_eval(position);

    if (score >= beta)
        return beta;
    if (alpha < score)
        alpha = score;

    MovesListNode* moves = movegen_pseudo_legal(position);
    MovesListNode *m, *t;
    bool           is_valid_move = false;

    m = (MovesListNode*) moves->next;

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
        score = -quiesce(position, -beta, -alpha);
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

int32_t alphabeta(Position* position, uint8_t depth, int32_t alpha, int32_t beta) {
    if (depth == 0)
        return quiesce(position, alpha, beta);

    int32_t best_value = -9999;
    int32_t score      = 0;

    MovesListNode* moves = movegen_pseudo_legal(position);
    MovesListNode *m, *t;
    bool           is_valid_move = false;

    m = (MovesListNode*) moves->next;

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

        score = -alphabeta(position, depth - 1, -beta, -alpha);

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
    return alphabeta(position, depth, -9999, 9999);
}
