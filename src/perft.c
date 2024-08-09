#include "include/perft.h"
#include "include/game.h"
#include "include/movegen.h"
#include "lib/logc/log.h"
#include <stdbool.h>
#include <stdlib.h>

uint64_t perft(Game* game, const uint8_t depth) {
    MovesListNode* candidate_moves;
    MovesListNode* move_node;
    u_int64_t      nodes         = 0;
    bool           is_valid_move = false;

    if (depth == 0)
    {
        return 1;
    }

    candidate_moves = generate_pseudo_legal_moves(game->position);
    move_node       = (MovesListNode*) candidate_moves->next;
    while (move_node != NULL)
    {
        is_valid_move = do_move(game, move_node->move, false);
        if (is_valid_move)
        {
            nodes += perft(game, depth - 1);
        }
        undo_move(game, move_node->move);
        move_node = (MovesListNode*) move_node->next;
    }
    return nodes;
}
