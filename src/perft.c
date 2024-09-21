#include "include/perft.h"
#include "include/chess.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint64_t perft(Position* position, const uint8_t depth) {
    MoveList* candidate_moves;
    Move*     move;
    uint64_t  nodes         = 0ULL;
    bool      is_valid_move = false;

    if (depth == 0)
        return 1ULL;

    candidate_moves = movegen_pseudo_legal(position);
    move            = (Move*) malloc(sizeof(Move));

    while (movegen_dequeue_move(candidate_moves, move))
    {
        is_valid_move = move_do(position, *move);
        if (is_valid_move)
        {
            nodes += perft(position, depth - 1);
        }
        move_undo(position);
    }

    free(candidate_moves);
    candidate_moves = NULL;

    free(move);
    move = NULL;

    return nodes;
}

void divide(Position* position, const uint8_t depth) {
    MoveList* candidate_moves = movegen_pseudo_legal(position);
    Move*     move            = (Move*) malloc(sizeof(Move));
    char      move_str[10];

    uint64_t nodes       = 0ULL;
    uint64_t total_nodes = 0ULL;

    while (movegen_dequeue_move(candidate_moves, move))
    {
        if (move_do(position, *move))
        {
            nodes = perft(position, depth - 1);
            total_nodes += nodes;
            move_to_str(move_str, *move);
            printf("%s %llu\n", move_str, nodes);
        }
        move_undo(position);
    }
    printf("\nPerft = %llu\n", total_nodes);
}
