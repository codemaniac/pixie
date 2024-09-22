#include "include/eval.h"
#include "include/chess.h"
#include "include/utils.h"

// clang-format off
static const int8_t POSITIONAL_SCORE_PAWN[64] = {
    100, 100, 100, 100, 100, 100, 100, 100,
     50,  50,  50,  50,  50,  50,  50,  50,
     25,  25,  25,  25,  25,  25,  25,  25,
     10,  10,  10,  10,  10,  10,  10,  10,
      5,   0,   0,  20,  20,   0,   0,   5,
      5,   0,   0,   5,   5,   0,   0,   5,
      0,   0,   0, -10, -10,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
};

static const int8_t POSITIONAL_SCORE_KNIGHT[64] = {
     -5,   0,   0,   0,   0,   0,   0,  -5,
     -5,   0,   0,  20,  20,   0,   0,  -5,
     -5,   0,  20,  25,  25,  20,   0,  -5,
      0,   0,  50,  25,  25,  50,   0,   0,
      0,   0,  10,  20,  20,  10,   0,   0,
     -5,  10,  20,   5,   5,  20,  10,  -5,
     -5,   0,   0,   5,   5,   0,   0,  -5,
     -5, -10,   0,   0,   0,   0, -10,  -5,
};

static const int8_t POSITIONAL_SCORE_BISHOP[64] = {
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,  10,  10,   0,   0,   0,
    0,  10,   0,  20,  20,   0,  10,   0,
    0,   0,  10,  20,  20,  10,   0,   0,
    0,   5,   0,   0,   0,   0,   5,   0,
    0,  20,   0,   0,   0,   0,  20,   0,
    0,   0, -10,   0,   0, -10,   0,   0,
};

static const int8_t POSITIONAL_SCORE_ROOK[64] = {
   50,  50,  50,  50,  50,  50,  50,  50,
   50,  50,  50,  50,  50,  50,  50,  50,
    0,   0,  10,  20,  20,  10,   0,   0,
    0,   0,  10,  20,  20,  10,   0,   0,
    0,   0,  10,  20,  20,  10,   0,   0,
    0,   0,  10,  20,  20,  10,   0,   0,
    0,   0,  10,  20,  20,  10,   0,   0,
    0,   0,   0,  20,  20,  10,   0,   0,
};

static const int8_t POSITIONAL_SCORE_QUEEN[64] = {
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   5,  20,  20,   5,   0,   0,
    0,   0,  10,  25,  25,  10,   0,   0,
    0,   0,  10,  25,  25,  10,   0,   0,
    0,  10,  20,  20,  20,  20,  10,   0,
    0,   0,   0,   5,   5,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
};

static const int8_t POSITIONAL_SCORE_KING[64] = {
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   5,   5,   5,   5,   0,   0,
    0,   5,   5,  10,  10,   5,   5,   0,
    0,   5,  10,  20,  20,  10,   5,   0,
    0,   5,  10,  20,  20,  10,   5,   0,
    0,   0,   5,  10,  10,   5,   0,   0,
    0,   5,   5,  -5,  -5,   0,   5,   0,
    0,   5,  10,   0, -15,   0,  25,   5,
};

static const int SQUARES_MIRRORED[64] = {
    A8, B8, C8, D8, E8, F7, G8, H8,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A1, B1, C1, D1, E1, F1, G1, H1,
};
// clang-format on

int32_t eval_position(const Position* position) {
    uint8_t wK = position->board.piece_count[WHITE_KING];
    uint8_t bK = position->board.piece_count[BLACK_KING];

    uint8_t wQ = position->board.piece_count[WHITE_QUEEN];
    uint8_t bQ = position->board.piece_count[BLACK_QUEEN];

    uint8_t wR = position->board.piece_count[WHITE_ROOK];
    uint8_t bR = position->board.piece_count[BLACK_ROOK];

    uint8_t wB = position->board.piece_count[WHITE_BISHOP];
    uint8_t bB = position->board.piece_count[BLACK_BISHOP];

    uint8_t wN = position->board.piece_count[WHITE_KNIGHT];
    uint8_t bN = position->board.piece_count[BLACK_KNIGHT];

    uint8_t wP = position->board.piece_count[WHITE_PAWN];
    uint8_t bP = position->board.piece_count[BLACK_PAWN];

    int8_t side_to_move = 1 - (2 * position->active_color);

    int32_t eval = (2000 * (wK - bK)) + (900 * (wQ - bQ)) + (500 * (wR - bR)) + (325 * (wB - bB))
                 + (300 * (wN - bN)) + (100 * (wP - bP));

    for (uint8_t p = 1; p < 15; p++)
    {
        if (p == BOARD_WHITE_PIECES_IDX || p == BOARD_BLACK_PIECES_IDX)
        {
            continue;
        }

        uint64_t bb = position->board.bitboards[p];

        while (bb)
        {
            uint8_t sq = utils_bit_bitscan_forward(&bb);

            switch (p)
            {
                case WHITE_PAWN:
                    eval += POSITIONAL_SCORE_PAWN[SQUARES_MIRRORED[sq]];
                    break;
                case WHITE_KNIGHT:
                    eval += POSITIONAL_SCORE_KNIGHT[SQUARES_MIRRORED[sq]];
                    break;
                case WHITE_BISHOP:
                    eval += POSITIONAL_SCORE_BISHOP[SQUARES_MIRRORED[sq]];
                    break;
                case WHITE_ROOK:
                    eval += POSITIONAL_SCORE_ROOK[SQUARES_MIRRORED[sq]];
                    break;
                case WHITE_QUEEN:
                    eval += POSITIONAL_SCORE_QUEEN[SQUARES_MIRRORED[sq]];
                    break;
                case WHITE_KING:
                    eval += POSITIONAL_SCORE_KING[SQUARES_MIRRORED[sq]];
                    break;
                case BLACK_PAWN:
                    eval -= POSITIONAL_SCORE_PAWN[sq];
                    break;
                case BLACK_KNIGHT:
                    eval -= POSITIONAL_SCORE_KNIGHT[sq];
                    break;
                case BLACK_BISHOP:
                    eval -= POSITIONAL_SCORE_BISHOP[sq];
                    break;
                case BLACK_ROOK:
                    eval -= POSITIONAL_SCORE_ROOK[sq];
                    break;
                case BLACK_QUEEN:
                    eval -= POSITIONAL_SCORE_QUEEN[sq];
                    break;
                case BLACK_KING:
                    eval -= POSITIONAL_SCORE_KING[sq];
                    break;
                default:
                    break;
            }
        }
    }

    eval *= side_to_move;

    return eval;
}
