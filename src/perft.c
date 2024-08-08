#include "include/perft.h"
#include "include/move.h"
#include "include/movegen.h"
#include "include/position.h"
#include <stdlib.h>

uint64_t perft(Position* position, const uint8_t depth) {
    MovesListNode* candidate_moves;
    MovesListNode* move_node;
    u_int64_t      nodes = 0;

    if (depth == 0)
    {
        return 1;
    }

    candidate_moves = generate_pseudo_legal_moves(position);
    move_node       = (MovesListNode*) candidate_moves->next;
    while (move_node != NULL)
    {
        do_move(position, move_node->move);
        if (!is_position_in_check(position))
        {
            nodes += perft(position, depth - 1);
        }
        undo_move(position, move_node->move);
        move_node = (MovesListNode*) move_node->next;
    }
    return nodes;
}
