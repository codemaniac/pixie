#include "include/board.h"
#include "include/piece.h"
#include <stdio.h>
#include <stdlib.h>

Board* create_empty_board(void) {
    Board* board = malloc(sizeof(Board));
    for (int i = 0; i < 120; i++)
    {
        if ((i < 21) || (i % 10 == 0) || (i % 10 == 9) || (i > 98))
        {
            board->squares[i] = __SQUARE_INVALID;
        }
        else
        {
            board->squares[i] = __SQUARE_EMPTY;
        }
    }
    return board;
}

void print_board(Board* board) {
    const char* pieces = ".PNBRQKpnbrqk";
    int         piece_id;
    char        piece;

    for (int rank = 7; rank >= 0; rank--)
    {
        for (int file = 0; file <= 7; file++)
        {
            piece_id = board->squares[__RF_TO_SQ(rank, file)];
            piece    = pieces[PIECE_GET_TYPE(piece_id)
                           + (PIECE_GET_COLOR(piece_id) * 6)];
            printf("%c ", piece);
        }
        printf("\n");
    }
}
