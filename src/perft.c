#include "include/perft.h"
#include "include/chess.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

uint64_t perft(Position* position, const uint8_t depth) {
    MoveList candidate_moves;
    Move     move;
    uint64_t nodes         = 0ULL;
    bool     is_valid_move = false;

    if (depth == 0)
        return 1ULL;

    movegen_pseudo_legal_all(position, &candidate_moves);

    while (movegen_dequeue_move(&candidate_moves, &move))
    {
        is_valid_move = move_do(position, move);
        if (is_valid_move)
        {
            nodes += perft(position, depth - 1);
        }
        move_undo(position);
    }

    return nodes;
}

void divide(Position* position, const uint8_t depth) {
    MoveList candidate_moves;
    Move     move;
    char     move_str[10];

    uint64_t nodes       = 0ULL;
    uint64_t total_nodes = 0ULL;

    movegen_pseudo_legal_all(position, &candidate_moves);

    while (movegen_dequeue_move(&candidate_moves, &move))
    {
        if (move_do(position, move))
        {
            nodes = perft(position, depth - 1);
            total_nodes += nodes;
            move_to_str(move_str, move);
            printf("%s %llu\n", move_str, nodes);
        }
        move_undo(position);
    }

    printf("\nPerft = %llu\n", total_nodes);
}
