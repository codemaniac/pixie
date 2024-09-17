#include "include/eval.h"
#include "include/chess.h"

int32_t eval_position(const Position* position) {
    uint8_t wK = position->board->piece_count[WHITE_KING];
    uint8_t bK = position->board->piece_count[BLACK_KING];

    uint8_t wQ = position->board->piece_count[WHITE_QUEEN];
    uint8_t bQ = position->board->piece_count[BLACK_QUEEN];

    uint8_t wR = position->board->piece_count[WHITE_ROOK];
    uint8_t bR = position->board->piece_count[BLACK_ROOK];

    uint8_t wB = position->board->piece_count[WHITE_BISHOP];
    uint8_t bB = position->board->piece_count[BLACK_BISHOP];

    uint8_t wN = position->board->piece_count[WHITE_KNIGHT];
    uint8_t bN = position->board->piece_count[BLACK_KNIGHT];

    uint8_t wP = position->board->piece_count[WHITE_PAWN];
    uint8_t bP = position->board->piece_count[BLACK_PAWN];

    int8_t side_to_move = 1 - (2 * position->active_color);

    int32_t eval = (2000 * (wK - bK)) + (900 * (wQ - bQ)) + (500 * (wR - bR)) + (300 * (wB - bB))
                 + (300 * (wN - bN)) + (100 * (wP - bP));

    eval *= side_to_move;

    return eval;
}
