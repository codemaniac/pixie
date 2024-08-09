#ifndef POSITION_H
#define POSITION_H

#include "board.h"
#include "piece.h"
#include <stdbool.h>
#include <stdint.h>

#define SQUARE_INVALID __SQUARE_INVALID
#define SQUARE_EMPTY __SQUARE_EMPTY
#define POS_IS_SQ_ON_BOARD(sq) (__IS_SQ_ON_BOARD(sq))
#define POS_RF_TO_SQ(r, f) (__RF_TO_SQ(r, f))
#define POS_SQ_TO_RANK(sq) (__SQ_TO_RANK(sq))
#define POS_SQ_TO_FILE(sq) (__SQ_TO_FILE(sq))
#define POS_IS_SQ_INVALID(pos, sq) (pos->board->squares[sq] == SQUARE_INVALID)
#define POS_IS_SQ_EMPTY(pos, sq) (pos->board->squares[sq] == SQUARE_EMPTY)
#define POS_ACTIVE_COLOR(pos) (pos->active_color)
#define POS_PIECE(pos, sq) (pos->board->squares[sq])
#define POS_SET_PIECE(pos, sq, p) (pos->board->squares[sq] = p)

typedef enum {
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8
} Rank;

typedef enum {
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H
} File;

enum {
    A1 = 21,
    B1,
    C1,
    D1,
    E1,
    F1,
    G1,
    H1,
    A2 = 31,
    B2,
    C2,
    D2,
    E2,
    F2,
    G2,
    H2,
    A3 = 41,
    B3,
    C3,
    D3,
    E3,
    F3,
    G3,
    H3,
    A4 = 51,
    B4,
    C4,
    D4,
    E4,
    F4,
    G4,
    H4,
    A5 = 61,
    B5,
    C5,
    D5,
    E5,
    F5,
    G5,
    H5,
    A6 = 71,
    B6,
    C6,
    D6,
    E6,
    F6,
    G6,
    H6,
    A7 = 81,
    B7,
    C7,
    D7,
    E7,
    F7,
    G7,
    H7,
    A8 = 91,
    B8,
    C8,
    D8,
    E8,
    F8,
    G8,
    H8,
};

typedef struct {
    Board*   board;
    Color    active_color;
    uint8_t  casteling_rights;
    uint8_t  enpassant_target;
    uint8_t  half_move_clock;
    uint8_t  full_move_number;
    uint64_t hash;
} Position;

bool is_square_attacked(const Position* position, const uint8_t sq);

bool is_position_in_check(const Position* position);

#endif
