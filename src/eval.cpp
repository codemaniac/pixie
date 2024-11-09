#include "include/eval.h"
#include "include/board.h"
#include "include/constants.h"
#include "include/position.h"
#include "include/utils.h"
#include <memory>

// clang-format off
static const int8_t POSITIONAL_SCORE_PAWN[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
     50,  50,  50,  50,  50,  50,  50,  50,
     10,  10,  20,  30,  30,  20,  10,  10,
      5,   5,  10,  25,  25,  10,   5,   5,
      0,   0,   0,  20,  20,   0,   0,   0,
      5,  -5, -10,   0,   0, -10,  -5,   5,
      5,  10,  10, -20, -20,  10,  10,   5,
      0,   0,   0,   0,   0,   0,   0,   0
};

static const int8_t POSITIONAL_SCORE_KNIGHT[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20,   0,   0,   0,   0, -20, -40,
    -30,   0,  10,  15,  15,  10,   0, -30,
    -30,   5,  15,  20,  20,  15,   5, -30,
    -30,   0,  15,  20,  20,  15,   0, -30,
    -30,   5,  10,  15,  15,  10,   5, -30,
    -40, -20,   0,   5,   5,   0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50,
};

static const int8_t POSITIONAL_SCORE_BISHOP[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -10,   0,   5,  10,  10,   5,   0, -10,
    -10,   5,   5,  10,  10,   5,   5, -10,
    -10,   0,  10,  10,  10,  10,   0, -10,
    -10,  10,  10,  10,  10,  10,  10, -10,
    -10,   5,   0,   0,   0,   0,   5, -10,
    -20, -10, -10, -10, -10, -10, -10, -20,
};

static const int8_t POSITIONAL_SCORE_ROOK[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
      5,  10,  10,  10,  10,  10,  10,   5,
     -5,   0,   0,   0,   0,   0,   0,  -5,
     -5,   0,   0,   0,   0,   0,   0,  -5,
     -5,   0,   0,   0,   0,   0,   0,  -5,
     -5,   0,   0,   0,   0,   0,   0,  -5,
     -5,   0,   0,   0,   0,   0,   0,  -5,
      0,   0,   0,   5,   5,   0,   0,   0
};

static const int8_t POSITIONAL_SCORE_QUEEN[64] = {
    -20, -10, -10,  -5,  -5, -10, -10, -20,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -10,   0,   5,   5,   5,   5,   0, -10,
     -5,   0,   5,   5,   5,   5,   0,  -5,
      0,   0,   5,   5,   5,   5,   0,  -5,
    -10,   5,   5,   5,   5,   5,   0, -10,
    -10,   0,   5,   0,   0,   0,   0, -10,
    -20, -10, -10,  -5,  -5, -10, -10, -20
};

static const int8_t POSITIONAL_SCORE_KING_MIDDLEGAME[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
     20,  20,   0,   0,   0,   0,  20,  20,
     20,  30,  10,   0,   0,  10,  30,  20
};

static const int8_t POSITIONAL_SCORE_KING_ENDGAME[64] = {
    -50, -40, -30, -20, -20, -30, -40, -50,
    -30, -20, -10,   0,   0, -10, -20, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -10,  30,  40,  40,  30, -10, -30,
    -30, -10,  30,  40,  40,  30, -10, -30,
    -30, -10,  20,  30,  30,  20, -10, -30,
    -30, -30,   0,   0,   0,   0, -30, -30,
    -50, -30, -30, -30, -30, -30, -30, -50
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

static BitBoard MASK_SQ_FILE[64];
static BitBoard MASK_SQ_RANK[64];
static BitBoard MASK_PAWN_ISOLATED[64];
static BitBoard MASK_PAWN_PASSED_WHITE[64];
static BitBoard MASK_PAWN_PASSED_BLACK[64];

static const int DOUBLE_PAWN_PENALTY   = -10;
static const int ISOLATED_PAWN_PENALTY = -10;
static const int PASSED_PAWN_BONUS[8]  = {0, 10, 30, 50, 75, 100, 150, 200};

void eval_init() {
    for (int sq = A1; sq <= H8; sq++)
    {
        const Square square = static_cast<Square>(sq);
        const Rank   rank   = BOARD_SQ_TO_RANK(square);
        const File   file   = BOARD_SQ_TO_FILE(square);

        // Fill MASK_SQ_FILE
        MASK_SQ_FILE[sq] = BOARD_MASK_A_FILE << file;

        // Fill MASK_SQ_RANK
        switch (rank)
        {
            case RANK_1 :
                MASK_SQ_RANK[sq] = BOARD_MASK_RANK_1;
                break;
            case RANK_2 :
                MASK_SQ_RANK[sq] = BOARD_MASK_RANK_1 << 8;
                break;
            case RANK_3 :
                MASK_SQ_RANK[sq] = BOARD_MASK_RANK_1 << 16;
                break;
            case RANK_4 :
                MASK_SQ_RANK[sq] = BOARD_MASK_RANK_4;
                break;
            case RANK_5 :
                MASK_SQ_RANK[sq] = BOARD_MASK_RANK_5;
                break;
            case RANK_6 :
                MASK_SQ_RANK[sq] = BOARD_MASK_RANK_5 << 8;
                break;
            case RANK_7 :
                MASK_SQ_RANK[sq] = BOARD_MASK_RANK_5 << 16;
                break;
            case RANK_8 :
                MASK_SQ_RANK[sq] = BOARD_MASK_RANK_8;
                break;
        }

        // Fill MASK_PAWN_ISOLATED
        MASK_PAWN_ISOLATED[sq] =
          bitboard_west_one(MASK_SQ_FILE[sq]) | bitboard_east_one(MASK_SQ_FILE[sq]);

        // Fill MASK_PAWN_PASSED_WHITE
        MASK_PAWN_PASSED_WHITE[sq] = bitboard_west_one(MASK_SQ_FILE[sq]) | MASK_SQ_FILE[sq]
                                   | bitboard_east_one(MASK_SQ_FILE[sq]);
        for (int r = RANK_1; r <= rank; r++)
            MASK_PAWN_PASSED_WHITE[sq] &= ~MASK_SQ_RANK[r * 8];

        // Fill MASK_PAWN_PASSED_BLACK
        MASK_PAWN_PASSED_BLACK[sq] = bitboard_west_one(MASK_SQ_FILE[sq]) | MASK_SQ_FILE[sq]
                                   | bitboard_east_one(MASK_SQ_FILE[sq]);
        MASK_PAWN_PASSED_BLACK[sq] ^= MASK_PAWN_PASSED_WHITE[sq];
        MASK_PAWN_PASSED_BLACK[sq] &= ~MASK_SQ_RANK[sq];
    }
}

int32_t eval_position(std::unique_ptr<Position>& position) {
    const Board board = position->get_board();

    const uint8_t wK = board.get_piece_count(WHITE_KING);
    const uint8_t bK = board.get_piece_count(BLACK_KING);

    const uint8_t wQ = board.get_piece_count(WHITE_QUEEN);
    const uint8_t bQ = board.get_piece_count(BLACK_QUEEN);

    const uint8_t wR = board.get_piece_count(WHITE_ROOK);
    const uint8_t bR = board.get_piece_count(BLACK_ROOK);

    const uint8_t wB = board.get_piece_count(WHITE_BISHOP);
    const uint8_t bB = board.get_piece_count(BLACK_BISHOP);

    const uint8_t wN = board.get_piece_count(WHITE_KNIGHT);
    const uint8_t bN = board.get_piece_count(BLACK_KNIGHT);

    const uint8_t wP = board.get_piece_count(WHITE_PAWN);
    const uint8_t bP = board.get_piece_count(BLACK_PAWN);

    int32_t eval = (20000 * (wK - bK)) + (900 * (wQ - bQ)) + (500 * (wR - bR)) + (330 * (wB - bB))
                 + (320 * (wN - bN)) + (100 * (wP - bP));

    bool is_end_game = false;

    if (wQ == 0 && bQ == 0)
        is_end_game = true;
    else if (wQ == 1 && bQ == 1 && wR == 0 && bR == 0)
    {
        if ((wN + wB) <= 1 && (bN + bB) <= 1)
            is_end_game = true;
    }

    int            doubled_pawns = 0, is_isolated_pawn = 0, is_passed_pawn = 0;
    const BitBoard wP_bb = board.get_bitboard(WHITE_PAWN);
    const BitBoard bP_bb = board.get_bitboard(BLACK_PAWN);

    for (uint8_t sq = A1; sq <= H8; sq++)
    {
        const Piece piece = board.get_piece(static_cast<Square>(sq));
        switch (piece)
        {
            case WHITE_PAWN :
                eval += POSITIONAL_SCORE_PAWN[SQUARES_MIRRORED[sq]];

                doubled_pawns = utils_bit_count1s(wP_bb & MASK_SQ_FILE[sq]);
                eval += (DOUBLE_PAWN_PENALTY * doubled_pawns);

                break;
            case WHITE_KNIGHT :
                eval += POSITIONAL_SCORE_KNIGHT[SQUARES_MIRRORED[sq]];
                break;
            case WHITE_BISHOP :
                eval += POSITIONAL_SCORE_BISHOP[SQUARES_MIRRORED[sq]];
                break;
            case WHITE_ROOK :
                eval += POSITIONAL_SCORE_ROOK[SQUARES_MIRRORED[sq]];
                break;
            case WHITE_QUEEN :
                eval += POSITIONAL_SCORE_QUEEN[SQUARES_MIRRORED[sq]];
                break;
            case WHITE_KING :
                eval += is_end_game ? POSITIONAL_SCORE_KING_ENDGAME[SQUARES_MIRRORED[sq]]
                                    : POSITIONAL_SCORE_KING_MIDDLEGAME[SQUARES_MIRRORED[sq]];
                break;
            case BLACK_PAWN :
                eval -= POSITIONAL_SCORE_PAWN[sq];

                doubled_pawns = utils_bit_count1s(bP_bb & MASK_SQ_FILE[sq]);
                eval -= (DOUBLE_PAWN_PENALTY * doubled_pawns);

                break;
            case BLACK_KNIGHT :
                eval -= POSITIONAL_SCORE_KNIGHT[sq];
                break;
            case BLACK_BISHOP :
                eval -= POSITIONAL_SCORE_BISHOP[sq];
                break;
            case BLACK_ROOK :
                eval -= POSITIONAL_SCORE_ROOK[sq];
                break;
            case BLACK_QUEEN :
                eval -= POSITIONAL_SCORE_QUEEN[sq];
                break;
            case BLACK_KING :
                eval -= is_end_game ? POSITIONAL_SCORE_KING_ENDGAME[sq]
                                    : POSITIONAL_SCORE_KING_MIDDLEGAME[sq];
                break;
            default :
                break;
        }
    }

    const int8_t side_to_move = 1 - (2 * position->get_active_color());
    eval *= side_to_move;

    return eval;
}

bool eval_is_end_game(std::unique_ptr<Position>& position) {
    const Board board = position->get_board();

    const uint8_t wQ = board.get_piece_count(WHITE_QUEEN);
    const uint8_t bQ = board.get_piece_count(BLACK_QUEEN);

    const uint8_t wR = board.get_piece_count(WHITE_ROOK);
    const uint8_t bR = board.get_piece_count(BLACK_ROOK);

    const uint8_t wB = board.get_piece_count(WHITE_BISHOP);
    const uint8_t bB = board.get_piece_count(BLACK_BISHOP);

    const uint8_t wN = board.get_piece_count(WHITE_KNIGHT);
    const uint8_t bN = board.get_piece_count(BLACK_KNIGHT);

    bool is_end_game = false;

    if (wQ == 0 && bQ == 0)
        is_end_game = true;
    else if (wQ == 1 && bQ == 1 && wR == 0 && bR == 0)
    {
        if ((wN + wB) <= 1 && (bN + bB) <= 1)
            is_end_game = true;
    }

    return is_end_game;
}
