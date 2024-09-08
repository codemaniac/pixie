#include "include/perft.h"
#include "include/chess.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint64_t perft(Position* position, const uint8_t depth) {
    MoveList*      candidate_moves;
    MovesListNode *move_node, *temp_move_node;
    uint64_t       nodes         = 0ULL;
    bool           is_valid_move = false;

    if (depth == 0)
        return 1ULL;

    candidate_moves = movegen_pseudo_legal(position);
    move_node       = (MovesListNode*) candidate_moves->head->next;

    while (move_node != NULL)
    {
        is_valid_move = move_do(position, move_node->move);
        if (is_valid_move)
        {
            nodes += perft(position, depth - 1);
        }
        move_undo(position);
        temp_move_node = move_node;
        move_node      = (MovesListNode*) move_node->next;
        free(temp_move_node);
    }

    return nodes;
}

void divide(Position* position, const uint8_t depth) {
    MoveList* candidate_moves = movegen_pseudo_legal(position);
    uint64_t  nodes           = 0ULL;
    uint64_t  total_nodes     = 0ULL;
    bool      is_valid_move   = false;

    MovesListNode* temp = (MovesListNode*) candidate_moves->head->next;
    MovesListNode* temp2;

    while (temp != NULL)
    {
        is_valid_move = move_do(position, temp->move);
        if (is_valid_move)
        {
            nodes = perft(position, depth - 1);
            total_nodes += nodes;

            print_move(temp->move);
            printf(" %llu\n", nodes);
        }
        move_undo(position);
        temp2 = temp;
        temp  = (MovesListNode*) temp->next;
        free(temp2);
    }
    printf("\nPerft = %llu\n", total_nodes);
}
